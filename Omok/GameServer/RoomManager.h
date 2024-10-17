#pragma once
class RoomManager
{
public:
	void Add(RoomRef room);
	void Remove(RoomRef room);
	void RemoveAll();
public:
	USE_LOCK
	set<RoomRef> _rooms;
	int32 _roomCount = 0;
};

extern RoomManager GRoomManager;
