/*
 * SDLNetSerevr.cpp
 *
 *  Created on: May 19, 2017
 *      Author: genaim
 */

#include "SDLNetServer.h"
#include <iostream>

using namespace std;

SDLNetServer::SDLNetServer() {
	ready_ = false;
}

SDLNetServer::~SDLNetServer() {
	closeConnection();
}

void SDLNetServer::closeConnection() {
	if (!ready_)
		return;
	SDLNet_TCP_Close(serverMasterSock_);
	SDLNet_FreeSocketSet(socketSet_);
	for (int i = 0; i < numOfPlayers_; i++)
		if (clientConnections_[i] != nullptr)
			delete clientConnections_[i];
	ready_ = false;
}

void SDLNetServer::setUpConnection() {

	// create the arrays of sockets and (pointers to) connections
	clientSockets_ = new TCPsocket[numOfPlayers_];
	clientConnections_ = new SDLNetConnection*[numOfPlayers_];

	for (int i = 0; i < numOfPlayers_; i++)
		clientConnections_[i] = nullptr;

	// create the socket set
	socketSet_ = SDLNet_AllocSocketSet(numOfPlayers_ + 1);
	if (socketSet_ == nullptr) {
		cout << "Failed to allocate the socket set: " << SDLNet_GetError()
				<< endl;
		exit(1);
	}

	// create an IP address that represents the server
	IPaddress ip;
	if (SDLNet_ResolveHost(&ip, nullptr, port_) == -1) {
		cout << "SDLNet_ResolveHost: " << SDLNet_GetError() << endl;
		exit(1);
	}

	// listen
	serverMasterSock_ = SDLNet_TCP_Open(&ip);
	if (serverMasterSock_ == NULL) {
		cout << "SDLNet_TCP_Open: " << SDLNet_GetError() << endl;
		exit(1);
	}

	// add the master socket to the server
	SDLNet_TCP_AddSocket(socketSet_, serverMasterSock_);

	// mark the connection as ready
	ready_ = true;
	cout << "Server is UP! " << endl;

}

void SDLNetServer::waitForPlayerToConnect() {

	cout << "Waiting for " << numOfPlayers_ << " players to connect" << endl;

	int connectedPlayers = 0;
	while (connectedPlayers < numOfPlayers_) {

		//listen  for a connection from a client (on the master socket)
		if (SDLNet_CheckSockets(socketSet_, 10000)
				> 0&& SDLNet_SocketReady(serverMasterSock_)) {

			// accept the connection
			TCPsocket newClientSock = SDLNet_TCP_Accept(serverMasterSock_);
			if (!newClientSock) {
				cout << "SDLNet_TCP_Accept: " << SDLNet_GetError() << endl;
				exit(1);
			}

			// add the new socket to the sockets set, store the socket
			// and create a corresponding SDLNetConnection
			SDLNet_TCP_AddSocket(socketSet_, newClientSock);
			clientSockets_[connectedPlayers] = newClientSock;
			clientConnections_[connectedPlayers] = new SDLNetConnection();
			clientConnections_[connectedPlayers]->connect(newClientSock);

			cout << "Connection with client " << connectedPlayers
					<< "has been established!" << endl;

			// send a message back to the client with its id, and the
			// number of clients
			game_msg_client_id connected_msg;
			connected_msg.size = sizeof(connected_msg);
			connected_msg.type = CONNECTED_;
			connected_msg.id = connectedPlayers;
			connected_msg.players = numOfPlayers_;

			if (!clientConnections_[connectedPlayers]->sendMessage(
					(game_msg*) &connected_msg)) {
				cout << "SDLNet_TCP_Send: " << SDLNet_GetError() << endl;
				delete clientConnections_[connectedPlayers];
			} else {
				connectedPlayers++;
			}
		}
	}

	cout << "All clients are connected!" << endl;

	// send a message to all clients informing them that all
	// clients are connect
	game_msg_t start_msg;
	start_msg.size = sizeof(start_msg);
	start_msg.type = GO_;

	sendToAllClients((game_msg*) &start_msg, -1);
}

void SDLNetServer::sendToAllClients(game_msg* msg, int except) {
	for (int i = 0; i < numOfPlayers_; i++) {
		if (i != except) {
			if (!clientConnections_[i]->sendMessage(msg)) {
				cout << "SDLNet_TCP_Send: " << SDLNet_GetError() << endl;
				exit(1);
			}
		}
	}
}

void SDLNetServer::processMessages() {

	cout << "The server is ready for forwarding messages!" << endl;

	bool inPlay = true;
	while (inPlay) {

		// wait for a connection from a client
		if (SDLNet_CheckSockets(socketSet_, 100000) <= 0)
			continue;

		// traverse all clients and check if they have sent data
		// in their corresponding sockets
		for (int i = 0; i < numOfPlayers_; i++) {
			if (SDLNet_SocketReady(clientSockets_[i])) {

				game_msg* msg = clientConnections_[i]->recvMessage();
				if (msg != nullptr) { // got a msg
					// uncomment the following if you want to simulate some latency
					// SDL_Delay(30);
					sendToAllClients(msg, i); // forward masg to all other clients
				} else { // something went wrong

					// send a stop message to all clients,  close connections, etc.
					game_msg_t stop_msg;
					stop_msg.size = sizeof(stop_msg);
					stop_msg.type = DISCONNECTED_;
					sendToAllClients((game_msg*) &stop_msg, i);

					for (int j = 0; j < numOfPlayers_; j++) {
						SDLNet_TCP_DelSocket(socketSet_, clientSockets_[j]);
						clientConnections_[j]->close();
						delete clientConnections_[j];
						clientConnections_[j] = nullptr;
					}
					inPlay = false;
					cout << "Closing connection to all clients " << i << endl;
					break;
				}
			}
		}
	}
}

void SDLNetServer::start(int port, int numOfPlayers) {

	numOfPlayers_ = numOfPlayers;
	port_ = port;
	stopped_ = false;

	setUpConnection();

	while (!stopped_) {
		waitForPlayerToConnect();
		processMessages();
	}

	closeConnection();
}
