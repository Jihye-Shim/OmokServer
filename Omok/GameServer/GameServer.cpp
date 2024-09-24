#include "pch.h"
#include "Listener.h"
#include "GameSession.h"
#include "GameSessionManager.h"

void HandleError(const char* cause){
	int32 errCode = ::WSAGetLastError();
	cout << cause << " ErrorCode : " << errCode << endl;
}

int32 threadCount = 5;// thread::hardware_concurrency();// (thread::hardware_concurrency()) *2 + 1;

int main()
{
	cout << "Concurrency: " << threadCount << endl;

	/* 초기화 */
	SocketUtils::Init();
	IocpCoreRef iocpCore = make_shared<IocpCore>();

	/* Register Accept */
	ListenerRef listener = make_shared<Listener>(SocketAddress(L"127.0.0.1", 7777), iocpCore, []() {return make_shared<GameSession>(); });
	ASSERT_CRASH(iocpCore->Register(listener));

	ASSERT_CRASH(listener->StartAccept());

	cout << "Ready to Accept!" << endl;

	/* WorkThread */
	vector<thread*> threads;
	for (int32 i = 0; i < threadCount; i++) {
		threads.push_back(new thread([&]() {
			while (true) {
				iocpCore->Dispatch();
			}
		}));
	}
	this_thread::sleep_for(3s);
	// Broadcast
	BYTE sendBuffer[100] = "Hello world";
	while (true) {
		//GSessionManager.Broadcast(sendBuffer);
		//this_thread::sleep_for(500ms);
	}
	for (auto t : threads)
		t->join();

	SocketUtils::Clear();
}
