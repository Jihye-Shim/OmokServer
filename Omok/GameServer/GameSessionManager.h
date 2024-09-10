#pragma once
class GameSession;
using GameSessionRef = shared_ptr<class GameSession>;
class GameSessionManager // 技记 包府
{
public:
	void Add(GameSessionRef session);
	void Remove(GameSessionRef session);
	void Broadcast(BYTE* buffer);

	int32 _sessionCount = 0;
private:
	//USE_LOCK
	set<GameSessionRef> _sessions;

};

extern GameSessionManager GSessionManager;