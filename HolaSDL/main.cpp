#include <vector>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <time.h>
#include <SDL.h>
#include <SDL_net.h>
#include "SDLNetServer.h"
#include "SDLNetConnection.h"
#include "VirtualClock.h"
#include "PingPong.h"

using namespace std;

//DEVIATION
Uint32 deviation(vector<Uint32> v) {
	Uint32 sum = 0;
	double E = 0;
	double ave;

	for (int i = 0; i < v.size(); i++)
		sum += v[i];
	ave = sum / v.size();

	for (int i = 0; i < v.size(); i++)
		E += (v[i] - ave) * (v[i] - ave);
	return (Uint32) sqrt(1 / v.size() * E);
}

void startServer(int port, int numberOfPlayers) {
	SDLNetServer server;

	server.start(port, numberOfPlayers);

}

void startClient(char *host, int port) {

	SDLNetConnection conn;
	Uint8 myId;
	Uint8 numOfPlayers;
	game_msg* msg = nullptr;
	Uint16* state = nullptr;
	Uint8* status = nullptr;
	VirtualClock clock;

	// connect to server
	conn.connect(host, port);

	// get a message, and check that it is one that includes client id, etc.
	msg = conn.recvMessage();
	if (((game_msg_t*) msg)->type != CONNECTED_) {
		std::cout << "Unexpected message!" << std::endl;
	}

	// retrieve info from msg
	myId = ((game_msg_client_id*) msg)->id;
	numOfPlayers = ((game_msg_client_id*) msg)->players;

	// print connected msg --- we cast Uint8 to int because otherwise it is printed
	// as char
	std::cout << "Connected with id: " << (int) myId << " out of "
			<< (int) numOfPlayers << " players" << std::endl;

	// create and initialize the states array
	state = new Uint16[numOfPlayers];
	status = new Uint8[numOfPlayers];
	for (int i = 0; i < numOfPlayers; i++) {
		state[i] = 0;
		status[i] = 'A';
	}

	// prepare state_change skeleton, so we don't fill it each time
	//
	game_msg_state_changed msg_state_changed;
	msg_state_changed.size = sizeof(msg_state_changed);
	msg_state_changed.id = myId;
	msg_state_changed.type = STATE_CHANGED_;

	// prepare status_change skeleton, so we don't fill it each time
	//
	game_msg_status_changed msg_status_changed;
	msg_status_changed.size = sizeof(msg_status_changed);
	msg_status_changed.id = myId;
	msg_status_changed.type = STATUS_CHANGED_;

	// wait for GO message
	msg = conn.recvMessage();
	if (((game_msg_t*) msg)->type != GO_) {
		std::cout << "Unexpected message!" << std::endl;
	}

	cout << "Starting clock synchronization!" << endl;
	game_msg_t sync_msg;
	if (myId == 0) {
		// Sync clock
		std::vector<Uint32> latencies;
		Uint32 lastTime;
		sync_msg.type = SYNC_REQUEST;
		sync_msg.size = sizeof(sync_msg);
		for (int i = 0; i < 101; i++) {
			lastTime = SDL_GetTicks();
			conn.sendMessage((game_msg*) &sync_msg);
			msg = conn.recvMessage(); // we assume the message is synch reply
			latencies.push_back( (SDL_GetTicks() - lastTime)/2 );
			SDL_Delay(20);
		}

		std::sort(latencies.begin(), latencies.end());
		Uint32 s = deviation(latencies);

		Uint32 sum = 0;
		Uint32 med = latencies[latencies.size() / 2];
		Uint32 l = med - s;
		Uint32 r = med + s;
		int n = 0;
		for (int i = 0; i < latencies.size(); i++) {
			if (latencies[i] >= l && latencies[i] <= r) {
				n++;
				sum += latencies[i];
			}
		}
		Uint32 latency = sum / n;
		game_msg_sync_done sync_done;
		sync_done.size = sizeof(sync_done);
		sync_done.type = SYNC_DONE;
		sync_done.latency = latency;
		clock.start();
		conn.sendMessage((game_msg*) &sync_done);
	} else {
		bool synched = false;
		sync_msg.type = SYNC_REPLY;
		sync_msg.size = sizeof(sync_msg);
		while (!synched) {
			msg = conn.recvMessage(); // we assume the message is synch reply
			if (((game_msg_t*) msg)->type == SYNC_REQUEST) {
				conn.sendMessage((game_msg*) &sync_msg);
			} else if (((game_msg_t*) msg)->type == SYNC_DONE) {
				synched = true;
				clock.start(((game_msg_sync_done*) msg)->latency);
			}
		}
	}
	cout << "Clock synchronization is done!" << endl;

	// initialize the seed of the random numbers generator
	srand(time(NULL));

	// last time state was changed (to change it every 5 seconds)
	Uint32 lastUpdated = clock.getTicks();

	msg = nullptr; // forget the pointer to last message

	bool stopped = false;
	while (!stopped) {

		// print state
		std::cout << (int)myId << " : " << clock.getTicks() << " : state :";
		for (int i = 0; i < numOfPlayers; i++) {
			cout << " [" << i << "]:" << (int) state[i] << ":"
					<< (char) status[i] << " ";
		}
		std::cout << std::endl;

		// process all messages that have been sent in the past
		if (msg == nullptr) {
			msg = conn.nbrecvMessage();
		}
		Uint32 last = clock.getTicks();
		bool gotSmthFromFuture = false;
		while (!gotSmthFromFuture && !stopped && msg != nullptr) {
			// if the message is disconnect, we exit put stopped to trueto exit
			if (((game_msg_t*) msg)->type == DISCONNECTED_) {
				stopped = true;
			} else if (last > ((game_msg_t*) msg)->timestamp) { // otherwise it is from a client, we check that it is from the past
				switch (((game_msg_t*) msg)->type) {
				case STATE_CHANGED_:
					state[((game_msg_state_changed*) msg)->id] =
							((game_msg_state_changed*) msg)->state;
					break;
				case STATUS_CHANGED_:
					status[((game_msg_status_changed*) msg)->id] =
							((game_msg_status_changed*) msg)->status;
					break;
				}
				msg = conn.nbrecvMessage();
			} else {
				gotSmthFromFuture = true;
			}

		}

		// every 3 seconds change state/status
		if (clock.getTicks() - lastUpdated > 100 && !stopped) {
			lastUpdated = clock.getTicks();

			// with 2% probability we exit the program
			if (rand() % 100 > 98)
				break;

			switch (rand() % 2) {
			case 0:
				state[myId] = rand() % 65536; // max value of two bytes is 65535
				msg_state_changed.state = state[myId];
				msg_state_changed.timestamp = clock.getTicks();
				conn.sendMessage((game_msg*) &msg_state_changed);
				break;
			case 1:
				status[myId] = 65 + (rand() % 26); // max value of two bytes is 65535
				msg_status_changed.status = status[myId];
				msg_status_changed.timestamp = clock.getTicks();
				conn.sendMessage((game_msg*) &msg_status_changed);
				break;
			}
		}

	}

	delete[] state;
	delete[] status;
	conn.close();
}


void main_fromcmd(int argc, char** argv) {
	if (argc == 4 && strcmp(argv[1], "server") == 0) {
		startServer(atoi(argv[2]), atoi(argv[3]));
	} else if (argc == 4 && strcmp(argv[1], "client") == 0) {
		startClient(argv[2], atoi(argv[3]));
	} else {
		cout << "Usage: " << endl;
		cout << " " << argv[0] << " server port num-of-players" << endl;
		cout << " " << argv[0] << " client host port" << endl;
	}
}

int main(int argc, char** argv) {
	if (SDLNet_Init() < 0) {
		cout << "SDLNet_init failed: " << SDLNet_GetError() << endl;
		exit(1);
	}
	main_fromcmd(argc,argv);
	PingPong g;
	g.start();
}

