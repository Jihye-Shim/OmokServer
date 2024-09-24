#include "pch.h"
#include "GameSession.h"

void GameSession::OnConnected(){
	GSessionManager.Add(static_pointer_cast<GameSession>(shared_from_this()));
	cout << "Connect!" << endl;
}

void GameSession::OnSend(int32 numOfBytes){
	cout << "Send Len: " << numOfBytes << endl;
}

void GameSession::OnRecv(int32 numOfBytes) {
	//cout << "Recv: " << _recvBuffer << endl;
	GSessionManager.Broadcast(_recvBuffer);
}

void GameSession::OnDisconnected() {
	GSessionManager.Remove(static_pointer_cast<GameSession>(shared_from_this()));
}