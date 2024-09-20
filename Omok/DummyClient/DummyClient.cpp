#include "pch.h"
#include "SocketUtils.h"
#include "Session.h"

void HandleError(const char* cause) {
	int32 errCode = ::WSAGetLastError();
	cout << cause << " ErrorCode : " << errCode << endl;
}

class SessionManager;
using ClientSessionRef = shared_ptr<class ClientSession>;
class SessionManager {
public:
	void Add(ClientSessionRef session) {
		LOCKGUARD
		sessions.insert(session);
		_sessionCount++;
	}
	void Remove(ClientSessionRef session) {
		LOCKGUARD
		sessions.erase(session);
		_sessionCount--;
	}
	int32 GetSessionCount() { return _sessionCount; }
	set<ClientSessionRef> sessions;
private:
	USE_LOCK
	int32 _sessionCount = 0;
};

SessionManager sessionManager;

class ClientSession : public Session {
public:
	~ClientSession() {
		cout << "~ClientSession " << sessionManager.GetSessionCount() << endl;
		
	}
	virtual void OnConnected() override {
		sessionManager.Add(static_pointer_cast<ClientSession>(shared_from_this()));
		cout << "Connected to Server!" << endl;
	}
	virtual void OnSend(int32 numOfBytes) override {
		//		cout << "Send!" << endl;
	}
	virtual void OnRecv(int32 numOfBytes) override {
		cout << "Recv: " << _recvBuffer << endl;
	}
	virtual void OnDisconnected() override {
		sessionManager.Remove(static_pointer_cast<ClientSession>(shared_from_this()));
	}
};

int32 sessionCount = 100;
int32 threadCount = 2;

int main()
{
	this_thread::sleep_for(1s);

	/* 초기화 */
	SocketUtils::Init();
	IocpCoreRef iocpCore = make_shared<IocpCore>();

	/* Connect*/
	for (int32 i = 0; i < sessionCount; i++) {
		ClientSessionRef session = make_shared<ClientSession>();
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
