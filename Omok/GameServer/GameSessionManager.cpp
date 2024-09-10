#include "pch.h"
#include "GameSessionManager.h"
#include "GameSession.h"

GameSessionManager GSessionManager;

void GameSessionManager::Add(GameSessionRef session)
{
	//WRITE_LOCK
	_sessions.insert(session);
	_sessionCount++;
}

void GameSessionManager::Remove(GameSessionRef session)
{
	//WRITE_LOCK
	_sessions.erase(session);
	_sessionCount--;
}

void GameSessionManager::Broadcast(BYTE* buffer)
{
	for (auto session : _sessions)
		session->Send(buffer);
}