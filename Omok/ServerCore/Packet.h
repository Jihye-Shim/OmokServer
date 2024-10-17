#pragma once

enum class PacketId : uint16_t{
	Login_Req = 21,
	Login_Res = 22,
	
	Lobby_Enter_Req = 31,
	Lobby_Enter_Res = 32,
	Lobby_Enter_Broad = 33,

	Lobby_Leave_Req = 34,
	Lobby_Leave_Res = 35,
	Lobby_Leave_Broad = 36,

	Lobby_Chat_Req = 37,
	Lobby_Chat_Res = 38,
	Lobby_Chat_Broad = 39,

	Room_Enter_Req = 41,
	Room_Enter_Res = 42,
	Room_Open_Broad = 43,

	Room_Leave_Req = 44,
	Room_Leave_Res = 45,
	Room_Full_Broad = 46,

	Game_Start_Req = 51,
	Game_Start_Res = 52,

	Game_Ready_Req = 53,
	Game_Ready_Res = 54,

	Game_Pos_Req = 53,
	Game_Pos_Res = 54,

	Game_Outcome_Broad = 55
};

struct PacketHeader { // 4byte
	uint16_t	totalSize;
	uint16_t	packetId;
};

struct PacketBase {
	uint32 errCode;
};
/*
* ErrCode
* 0: None
* 1: ExistName
*/

const int MAX_USER_NAME_SIZE = 64;
const int MAX_CHAT_SIZE = 1024;

/*------------------
		Login
-------------------*/
struct PacketLogInReq
{
	BYTE userName[MAX_USER_NAME_SIZE] = {0, }; // 일단 이름만
};

struct PacketLogInRes : PacketBase {
	vector<BYTE*> userList;
	vector<BYTE*> roomList;
};

/*------------------
		Lobby
-------------------*/
struct PacketLobbyEnterReq {
	BYTE userName[MAX_USER_NAME_SIZE] = { 0, };
};

struct PacketLobbyEnterRes : PacketBase{
};

struct PacketLobbyEnterBroad {
	BYTE userName[MAX_USER_NAME_SIZE] = { 0, };
};

struct PacketLobbyLeaveReq {
	BYTE userName[MAX_USER_NAME_SIZE] = { 0, };
};

struct PacketLobbyLeaveRes : PacketBase {
	BYTE userName[MAX_USER_NAME_SIZE] = { 0, };
};

struct PacketLobbyLeaveBroad {
	BYTE userName[MAX_USER_NAME_SIZE] = { 0, };
};

/*------------------
		Chat
-------------------*/
struct PacketLobbyChatReq {
	BYTE userName[MAX_USER_NAME_SIZE] = { 0, };
	BYTE msg[MAX_CHAT_SIZE] = { 0, };
};

struct PacketLobbyChatRes : PacketBase {
};

struct PacketLobbyChatBroad {
	BYTE userName[MAX_USER_NAME_SIZE] = { 0, };
	BYTE msg[MAX_CHAT_SIZE] = { 0, };
};

/*------------------
		Room
-------------------*/
struct PacketRoomEnterReq {
	BYTE userName[MAX_USER_NAME_SIZE] = { 0, };
	BYTE roomName[MAX_USER_NAME_SIZE] = { 0, };
};

struct PacketRoomEnterRes : PacketBase{
	BYTE user1[MAX_USER_NAME_SIZE] = { 0, };
	BYTE user2[MAX_USER_NAME_SIZE] = { 0, };
	BYTE roomName[MAX_USER_NAME_SIZE] = { 0, };
};
struct PacketRoomOpenBroad {
	BYTE roomName[MAX_USER_NAME_SIZE] = { 0, };
};

struct PacketRoomFullBroad {
	BYTE roomName[MAX_USER_NAME_SIZE] = { 0, };
};

struct PacketRoomLeaveReq {
};

struct PacketRoomLeaveRes: PacketBase {

};

/*------------------
		Game
-------------------*/

struct PacketGamePosReq {
	uint16_t x;
	uint16_t y;
};
struct PacketGamePosRes :PacketBase{
	uint16_t x;
	uint16_t y;
};

struct PacketGameOutcomeBroad {
	uint16_t win;
};

template<typename T>
PacketHeader MakeHeader(PacketId id) {
	PacketHeader header;
	header.totalSize = sizeof(PacketHeader) + sizeof(T);
	header.packetId = static_cast<uint16_t>(id);
	return header;
}