#include "pch.h"
#include "SocketUtils.h"
#include "Session.h"

void HandleError(const char* cause) {
	int32 errCode = ::WSAGetLastError();
	cout << cause << " ErrorCode : " << errCode << endl;
}

void WorkThread(HANDLE iocpHandle) {
	while (true) {
		DWORD numOfBytes;
		IocpEvent* iocpEvent = nullptr;
		ULONG_PTR key;
		if (::GetQueuedCompletionStatus(iocpHandle, OUT & numOfBytes, OUT & key, OUT(LPOVERLAPPED*)(&iocpEvent), INFINITE)) {
			if (numOfBytes == 0) // disconnect???
				continue;
			shared_ptr<Session> owner = iocpEvent->owner;
			owner->Dispatch(iocpEvent, numOfBytes);
		}
	}
}

class ClientSession : public Session {
public:
	void OnSend() override {
//		cout << "Send!" << endl;
		RegisterRecv();
	}
	void OnRecv() override {
		cout << _recvBuffer << endl;
	}
};

int main()
{
	this_thread::sleep_for(1s);

	SocketUtils::Init();

	SOCKET clientSocket = SocketUtils::CreateSocket();
	SocketAddress serverAddress = SocketAddress(L"127.0.0.1", 7777);

	if (false == SocketUtils::Connect(clientSocket, &serverAddress)) {
		HandleError("Connect");
		return 0;
	}
	cout << "Connect to Server!" << endl;
	auto clientSession = std::make_shared<ClientSession>();
	clientSession->_socket = clientSocket;
	clientSession->SetSocketAddress(serverAddress);

	HANDLE iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
	ASSERT_CRASH(iocpHandle != INVALID_HANDLE_VALUE);
	
	::CreateIoCompletionPort((HANDLE)clientSession->GetHandle(), iocpHandle, 0, 0);

	// CP 작업용 스레드
	vector<thread*> threads;
	thread *t;
	for (int i = 0; i < 2; i++) {
		t = new thread(WorkThread, iocpHandle);
		threads.push_back(t);
	}
	cout << "Your name: ";
	char name[100];
	cin >> name;
	clientSession->_name = name;
	//Main: Input thread
	char sendBuffer[1000] = "";
	while (true) {
		cout << "Input: ";
		cin >> sendBuffer;
		::memcpy(clientSession->_sendBuffer, sendBuffer, BUFSIZE);
		clientSession->RegisterSend();
	//	this_thread::sleep_for(1s);
	}

	for (auto t : threads)
		t->join();

	SocketUtils::Clear();
}
