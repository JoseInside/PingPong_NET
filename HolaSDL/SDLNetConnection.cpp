/*
 * SDLNetConnection.cpp
 *
 *  Created on: May 19, 2017
 *      Author: genaim
 */

#include "SDLNetConnection.h"

SDLNetConnection::SDLNetConnection() {
	ready_ = false;
}

SDLNetConnection::~SDLNetConnection() {
}

int SDLNetConnection::connect(char* host, int port) {
	close();
	IPaddress ip;

	if (SDLNet_ResolveHost(&ip, host, port) == -1) {
		return -1;
	}

	TCPsocket sock = SDLNet_TCP_Open(&ip);
	if (!sock) {
		return -2;
	}

	return connect(sock);
}

int SDLNetConnection::connect(TCPsocket sock) {
	sock_ = sock;

	socketSet_ = SDLNet_AllocSocketSet(1);
	if (socketSet_ == nullptr) {
		return -3;
	}

	SDLNet_TCP_AddSocket(socketSet_, sock_);

	ready_ = true;
	return 0;

}

bool SDLNetConnection::sendMessage(game_msg* msg) {
	if (!ready_)
		return false;

	// if return true if succeeded to send all bytes, otherwise false
	return SDLNet_TCP_Send(sock_, msg, msg->size) == msg->size;
}

game_msg* SDLNetConnection::recvMessage() {
	if (!ready_)
		return nullptr;

	// start reading the header, if failed return 'nullptr'
	int n = SDLNet_TCP_Recv(sock_, (Uint8*) &buffer_, headerSize);
	if (n <= 0)
		return nullptr;

	// read the rest of the header, return 'nullptr' if failed
	while (n < headerSize) {
		int y = SDLNet_TCP_Recv(sock_, (Uint8*)&buffer_ + n, headerSize - n);
		if (y <= 0) {
			return nullptr;
		} else {
			n = n + y;
		}
	}

	// read the rest of the message, return 'nullptr' if failed
	while (n < buffer_.size) {
		int y = SDLNet_TCP_Recv(sock_, (char*) &buffer_ + n,
				buffer_.size - n);
		if (y <= 0) {
			return nullptr;
		} else {
			n = n + y;
		}
	}

	return &buffer_;
}

game_msg* SDLNetConnection::nbrecvMessage() {

	if (!ready_)
		return nullptr;

	int activeSockets = SDLNet_CheckSockets(socketSet_, 0);

	// if the socket is ready, try to read in a blocking mode
	if (activeSockets > 0 && SDLNet_SocketReady(sock_)) {
		return recvMessage();
	} else {
		return nullptr;
	}
}

void SDLNetConnection::close() {
	if (ready_) {
		SDLNet_TCP_Close(sock_); // close socket
		ready_ = false;
		SDLNet_FreeSocketSet(socketSet_); // free the socket set
	}
}
