#include "pch.h"
#include "GameSessionManager.h"
#include "GameSession.h"
#include "RoomManager.h"

GameSessionManager GSessionManager;

void GameSessionManager::Add(GameSessionRef session)
{
	LOCKGUARD
	_sessions.insert(session);
	_sessionCount++;
}

void GameSessionManager::Remove(GameSessionRef session)
{
	LOCKGUARD
	_sessions.erase(session);
	_sessionCount--;
	if (_sessionCount == 0)
		GRoomManager.RemoveAll();
}

void GameSessionManager::Broadcast(OutputMemoryStreamRef outStream)
{
	for (auto session : _sessions)
		session->Send(outStream);
}