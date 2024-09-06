#include "pch.h"
#include "SocketUtils.h"
#include "Session.h"

set<shared_ptr<Session>> sessions;

void HandleError(const char* cause){
	int32 errCode = ::WSAGetLastError();
	cout << cause << " ErrorCode : " << errCode << endl;
}

void WorkThread(HANDLE iocpHandle) {
	while (true) {
		DWORD numOfBytes;
		IocpEvent* iocpEvent = nullptr;
		ULONG_PTR key;
		if(::GetQueuedCompletionStatus(iocpHandle, OUT &numOfBytes, OUT &key, OUT (LPOVERLAPPED*)(&iocpEvent), INFINITE)) {
			if (numOfBytes == 0) // disconnect???
				continue;
			shared_ptr<Session> owner = iocpEvent->owner;
			owner->Dispatch(iocpEvent, numOfBytes);
		}
	}
}

void Broadcast(BYTE* buffer) {
	for (auto session : sessions)
		session->Send(buffer);
}

class GameSession : public Session {
public:
	void OnSend() override {
//		cout << "Send Message!" << endl;
	}
	void OnRecv() override {
		cout << "Recv: " << _recvBuffer << endl;
		//::memcpy(_sendBuffer, _recvBuffer, BUFSIZE);
		Broadcast(_recvBuffer);
	}
};



int main()
{
	SocketUtils::Init();

	SOCKET listenSocket = SocketUtils::CreateSocket();

	if (false == SocketUtils::BindAnyAddress(listenSocket, 7777)) {
		HandleError("Bind");
		return 0;
	}

	if (false == SocketUtils::Listen(listenSocket)){
		HandleError("Listen");
		return 0;
	}
	cout << "Server: Ready to Accept" << endl;

	

	HANDLE iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
	ASSERT_CRASH(iocpHandle != INVALID_HANDLE_VALUE);
	
	// CP 작업용 스레드
	vector<thread*> threads;
	thread* t;
	for (int i = 0; i < 2; i++) {
		t = new thread(WorkThread, iocpHandle);
		threads.push_back(t);
	}

	//Main -> Accept thread
	while (true) {
		SOCKADDR_IN clientAddress;
		int32 addLen = sizeof(clientAddress);
		SOCKET clientSocket = ::accept(listenSocket, OUT(SOCKADDR*)(&clientAddress), &addLen);
		if (clientSocket == INVALID_SOCKET)
			continue; // 나중에 비동기로
		cout << " Accept!" << endl;
		auto session = std::make_shared<GameSession>();
		session->_socket = clientSocket;
		session->SetSocketAddress(SocketAddress(clientAddress));
		sessions.insert(session);
		::CreateIoCompletionPort((HANDLE)session->GetHandle(), iocpHandle, 0, 0);

		session->RegisterRecv();
	}
	for (auto t : threads)
		t->join();

	SocketUtils::Clear();
}
