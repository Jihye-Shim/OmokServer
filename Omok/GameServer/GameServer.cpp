#include "pch.h"
#include "Listener.h"
#include "GameSession.h"
#include "GameSessionManager.h"

void HandleError(const char* cause){
	int32 errCode = ::WSAGetLastError();
	cout << cause << " ErrorCode : " << errCode << endl;
}

int32 threadCount = 5;// thread::hardware_concurrency(); // (thread::hardware_concurrency()) *2 + 1;

int main()
{

	/* 초기화 */
	SocketUtils::Init();
	IocpCoreRef GIocpCore = make_shared<IocpCore>();
	setlocale(LC_ALL, "");

	/* Register Accept */
	ListenerRef GListener = make_shared<Listener>(SocketAddress(L"127.0.0.1", 7777), GIocpCore, []() {return make_shared<GameSession>(); });
	ASSERT_CRASH(GIocpCore->Register(GListener));
	ASSERT_CRASH(GListener->StartAccept());

	cout << "Ready to Accept!" << endl;

	/* WorkThread */
	vector<thread*> threads;
	for (int32 i = 0; i < threadCount; i++) {
		threads.push_back(new thread([&]() {
			while (true) {
				GIocpCore->Dispatch();
			}
		}));
	}

	for (auto t : threads)
		t->join();

	SocketUtils::Clear();
}
