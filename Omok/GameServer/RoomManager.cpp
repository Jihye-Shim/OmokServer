#include "pch.h"
#include "RoomManager.h"

RoomManager GRoomManager;
void RoomManager::Add(RoomRef room)
{
	LOCKGUARD
	_rooms.insert(room);
	_roomCount++;
}

void RoomManager::Remove(RoomRef room)
{
	LOCKGUARD
	_rooms.erase(room);
	_roomCount--;
}

void RoomManager::RemoveAll()
{
	_rooms.clear();
}
