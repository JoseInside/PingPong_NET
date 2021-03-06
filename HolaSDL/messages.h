/*
 * messages.h
 *
 *  Created on: May 22, 2017
 *      Author: genaim
 */

#ifndef MESSAGES_H_
#define MESSAGES_H_


// make sure MSG_SIZE_T is enough to hold the size of game_msg
static const int MAX_SIZE = 255;
typedef Uint16 MSG_SIZE_T;
static const int headerSize = sizeof(MSG_SIZE_T);

// this enum is used for the type of messages, in principle we
// require that it fits in one byte only
enum MsgType {
	CONNECTED_ = 0,
	DISCONNECTED_ = 1,
	GO_ = 2,
	STATE_CHANGED_ = 3,
	STATUS_CHANGED_ = 4,
	SYNC_REQUEST = 5,
	SYNC_REPLY = 6,
	SYNC_DONE
};

// generic message -- used only by SDLNetConnection, all others
// should send typed messages
typedef struct {
	MSG_SIZE_T size;
	Uint8 data[MAX_SIZE];
} game_msg;

// generic typed message
typedef struct {
	MSG_SIZE_T size;
	Uint8 type;
	Uint32 timestamp;
} game_msg_t;

// a message used by the server to send the id to the client,
// and also how many client are in the game
typedef struct {
	MSG_SIZE_T size;
	Uint8 type;
	Uint8 players;
	Uint8 id;
} game_msg_client_id;

typedef struct {
	MSG_SIZE_T size;
	Uint8 type;
	Uint32 timestamp;
	Uint32 latency;
} game_msg_sync_done;


// APPLICATION DEPENDENY MESSAGES
//
//  our demo application consists of a server, and several
//  clients.

//  Each client maintain a state (a number) that
//  changes every 5 seconds. When changed it send its state
//  all client so they sync.
//
//  Each client maintain a status (5 chars). When changed it
//  send its status to clients so they sync.

// used by the client to send its state
typedef struct {
	MSG_SIZE_T size;
	Uint8 type;
	Uint32 timestamp;
	Uint8 id;
	Uint16 state;
} game_msg_state_changed;

// used by the client to send its first part of the state
typedef struct {
	MSG_SIZE_T size;
	Uint8 type;
	Uint32 timestamp;
	Uint8 id;
	Uint8 status; // could be char also
} game_msg_status_changed;


#endif /* MESSAGES_H_ */
