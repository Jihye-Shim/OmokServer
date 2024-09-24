#pragma once
#include "Session.h"
#include "GameSessionManager.h"
class GameSession : public Session {
public:
	~GameSession() {
		cout << "~GameSession " << GSessionManager.GetSessionCount() << endl;
	}
	virtual void OnConnected() override;
	virtual void OnSend(int32 numOfBytes) override;
	virtual void OnRecv(int32 numOfBytes) override;
	virtual void OnDisconnected() override;
};

