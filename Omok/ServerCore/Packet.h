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

	Game_Enter_Req = 41,
	Game_Enter_Res = 42,

	Game_Leave_Req = 43,
	Game_Leave_Res = 44
};

struct PacketHeader { // 4byte
	uint16_t	totalSize;
	uint16_t	packetId;
};

struct PacketBase {
	uint32 errCode;
};

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

struct PacketGameEnterReq {

};

struct PacketGameEnterRes {

};

struct PacketGameLeaveReq {

};

struct PacketGameLeaveRes {

};

template<typename T>
PacketHeader MakeHeader(PacketId id) {
	PacketHeader header;
	header.totalSize = sizeof(PacketHeader) + sizeof(T);
	header.packetId = static_cast<uint16_t>(id);
	return header;
}