#include "pch.h"
#include "SocketUtils.h"
#include "Session.h"

void HandleError(const char* cause) {
	int32 errCode = ::WSAGetLastError();
	cout << cause << " ErrorCode : " << errCode << endl;
}

class ClientSession : public Session {
public:
	~ClientSession() {
		cout << "~ClientSession" << endl;
	}
	virtual void OnConnected() override {
		cout << "Connected to Server!" << endl;
	}
	virtual void OnSend(int32 numOfBytes) override {
//		cout << "Send!" << endl;
	}
	virtual void OnRecv(int32 numOfBytes) override {
		cout << "Recv Len:" << numOfBytes << endl;
	}
	virtual void OnDisconnected() override {
		//
	}
};
using ClientSessionRef = shared_ptr<ClientSession>;

int32 sessionCount = 1000;
int32 threadCount = 2;

int main()
{
	this_thread::sleep_for(1s);

	/* 초기화 */
	SocketUtils::Init();
	IocpCoreRef iocpCore = make_shared<IocpCore>();

	/* Connect*/
	vector<ClientSessionRef> sessions;
	for (int32 i = 0; i < sessionCount; i++) {
		ClientSessionRef session = make_shared<ClientSession>();
		sessions.push_back(session);
		iocpCore->Register(session);
		ASSERT_CRASH(session->Connect(SocketAddress(L"127.0.0.1", 7777)));
	}

	/* workthread */ 
	vector<thread*> threads;
	for (int32 i = 0; i < threadCount; i++) {
		threads.push_back(new thread([&]() {
			while (true) {
				iocpCore->Dispatch();
			}
		}));
	}

	for (auto t : threads)
		t->join();

	SocketUtils::Clear();
}
