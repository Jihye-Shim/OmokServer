#pragma once
class GameSession;
using GameSessionRef = shared_ptr<class GameSession>;
class GameSessionManager // 技记 包府
{
public:
	void Add(GameSessionRef session);
	void Remove(GameSessionRef session);
	void Broadcast(OutputMemoryStreamRef outStream);
	int32 GetSessionCount() { return _sessionCount; }

private:
	USE_LOCK
	set<GameSessionRef> _sessions;
	int32 _sessionCount = 0;
};

extern GameSessionManager GSessionManager;