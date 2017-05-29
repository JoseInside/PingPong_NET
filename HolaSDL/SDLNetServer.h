#ifndef SDLNETSERVER_H_
#define SDLNETSERVER_H_

#include <SDL_net.h>
#include "messages.h"
#include "SDLNetConnection.h"

class SDLNetServer {
public:
	SDLNetServer();
	virtual ~SDLNetServer();
	void start(int port, int numOfPlayers);
	void stop();

private:
	void setUpConnection();
	void closeConnection();
	void waitForPlayerToConnect();
	void processMessages();
	void sendToAllClients(game_msg* msg, int except);

	// for master socket
	TCPsocket serverMasterSock_;
	TCPsocket* clientSockets_;
	SDLNetConnection** clientConnections_;

	SDLNet_SocketSet socketSet_;
	int numOfPlayers_;
	int port_;
	bool stopped_;
	bool ready_;
};

#endif /* SDLNETSERVER_H_ */
