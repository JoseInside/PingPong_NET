#ifndef SDLNETCONNECTION_H_
#define SDLNETCONNECTION_H_

#include <string>
#include <SDL_net.h>
#include "messages.h"

class SDLNetConnection {
public:
	SDLNetConnection();
	virtual ~SDLNetConnection();

	// open a connection to host:port
	int connect(char* host, int port);

	// use 'sock' as the underlying socket for the connection. This useful if
	// we have a socket and we want to use send/recv of this class
	int connect(TCPsocket sock);

	// send a message
	bool sendMessage(game_msg* msg);

	// receive  message, blocking, returns nullptr if failed
	game_msg* recvMessage();

	// receive message, non-blocking, returns nullptr if failed
	game_msg* nbrecvMessage();

	// close the connection
	void close();

private:
	// indicates of the connection is ready
	bool ready_;

	// the underlying socket
	TCPsocket sock_;

	// A socket set to be used in non-blocking receive
	SDLNet_SocketSet socketSet_;

	// a buffer to be used for receiving messages
	game_msg buffer_;
};

#endif /* SDLNETCONNECTION_H_ */
