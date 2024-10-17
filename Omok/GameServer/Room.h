#pragma once
#include "GameSession.h"
#include "RoomManager.h"

class Room : public enable_shared_from_this<Room>
{
public:
	~Room() {
		cout << "~Room " << GRoomManager._roomCount << endl;
	}
	uint16 AddStone(BYTE* name, uint32 posX, uint32 posY);
	uint16 IsWin(uint32 stone, uint32 posX, uint32 posY);
	bool isInSize(uint32 x, uint32 y);

public:
	IocpObjectRef _session1 = nullptr;
	IocpObjectRef _session2 = nullptr;
	BYTE _name[MAX_USER_NAME_SIZE] = { 0, };
	uint32 _start = 0;
	uint32 _ready = 0;
	uint32 _board[19][19] = { 0, };
};

