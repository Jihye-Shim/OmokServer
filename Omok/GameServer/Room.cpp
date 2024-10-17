#include "pch.h"
#include "Room.h"
#include "GameSession.h"
#define BOARDSIZE 19
uint16 Room::AddStone(BYTE* name, uint32 posX, uint32 posY)
{
	uint32 stone;
	BYTE user[MAX_USER_NAME_SIZE] = { 0, };
	::memcpy(user, static_pointer_cast<GameSession>(_session1)->_name, MAX_USER_NAME_SIZE);
	if (::memcmp(name, static_pointer_cast<GameSession>(_session1)->_name, sizeof(name)) == 0) 
		stone = 1;
	else
		stone = 2;
	_board[posX][posY] = stone;
	return IsWin(stone, posX, posY);
}

uint16 Room::IsWin(uint32 stone, uint32 posX, uint32 posY)
{
	bool right;
	bool left;
	//1. \¸ð¾ç
	for (int i = 0; i < 5; i++) {
		uint32 x = posX - 4 + i;
		uint32 y = posY - 4 + i;
		int32 len = 0;
		for (int j = 0; j < 5; j++) {
			if (isInSize(x+j, y+j) && _board[x+j][y+j] == stone)
				len++;
		}
		if (len == 5)
			return stone;
	}

	//2. /
	for (int i = 0; i < 5; i++) {
		uint32 x = posX - 4 + i;
		uint32 y = posY + 4 - i;
		int32 len = 0;
		for (int j = 0; j < 5; j++) {
			if (isInSize(x+j, y-j) && _board[x+j][y-j] == stone)
				len++;
		}
		if (len == 5)
			return stone;
	}

	//3. ¤Ñ
	for (int i = 0; i < 5; i++) {
		uint32 x = posX - 4 + i;
		uint32 y = posY;
		int32 len = 0;
		for (int j = 0; j < 5; j++) {
			if (isInSize(x+j, y) && _board[x+j][y] == stone)
				len++;
		}
		if (len == 5)
			return stone;
	}
	//4. |
	for (int i = 0; i < 5; i++) {
		uint32 x = posX;
		uint32 y = posY-4+i;
		int32 len = 0;
		for (int j = 0; j < 5; j++) {
			if (isInSize(x, y+j) && _board[x][y+j] == stone)
				len++;
		}
		if (len == 5)
			return stone;
	}
	return 0;
}

bool Room::isInSize(uint32 x, uint32 y)
{
	if (x < 0 || x >= BOARDSIZE)
		return false;
	if (y < 0 || y >= BOARDSIZE)
		return false;
	return true;
}