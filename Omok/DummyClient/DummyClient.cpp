#include "pch.h"
#include "SocketUtils.h"
#include "Session.h"
#pragma warning(disable : 4996)
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
	//	cout << "Send!" << endl;
	}
	virtual void OnRecv(int32 numOfBytes) override {
	//	cout << "Recv: " << _recvBuffer << endl;
		InputMemoryStream inStream(_recvBuffer, numOfBytes);
		PacketHeader header;
		inStream.Read(header.totalSize);
		if (header.totalSize > numOfBytes) {
			//패킷 다 안 왔을 때 처리
		}
		inStream.Read(header.packetId);

		//패킷 처리
		PacketId id = static_cast<PacketId>(header.packetId);
		if (id == PacketId::Login_Res) {
			cout << "Success to Enter Lobby" << endl;
		}
		else if (id == PacketId::Lobby_Enter_Broad) {
			PacketLobbyEnterBroad pkt;
			inStream.Read(pkt.userName, MAX_USER_NAME_SIZE);	
			wcout << Utf8ToWChar(pkt.userName) << "님이 입장하셨습니다." << endl;
		}
		else if (id == PacketId::Lobby_Chat_Broad) {
			PacketLobbyChatBroad pkt;
			inStream.Read(pkt.userName, MAX_USER_NAME_SIZE);
			inStream.Read(pkt.msg, MAX_CHAT_SIZE);
			cout << Utf8ToWChar(pkt.userName) << ": " << pkt.msg << endl;
		}
	}
	virtual void OnDisconnected() override {
		sessionManager.Remove(static_pointer_cast<ClientSession>(shared_from_this()));
	}
	WCHAR* Utf8ToWChar(BYTE* utf8)
	{
		string utf8ToString(reinterpret_cast<char*>(utf8), strlen((char*)utf8));
		int32 sLen = MultiByteToWideChar(CP_UTF8, 0, utf8ToString.c_str(), utf8ToString.length(), NULL, 0);
		WCHAR* wideString = new WCHAR[sLen + 1];
		MultiByteToWideChar(CP_UTF8, 0, utf8ToString.c_str(), utf8ToString.length(), OUT wideString, sLen);
		wideString[sLen] = '\0';
		return wideString;
	}
};

int32 sessionCount = 50;
int32 threadCount = 2;

int main()
{
	//this_thread::sleep_for(1s);
	//size_t a;
	//int32 b;
	//cout << sizeof(a) << sizeof(b) << endl;

	//this_thread::sleep_for(100s);

	/* 초기화 */
	SocketUtils::Init();
	IocpCoreRef iocpCore = make_shared<IocpCore>();

	 /* Connect*/
	 //multi-dummy
	for (int32 i = 0; i < sessionCount; i++) {
		ClientSessionRef session = make_shared<ClientSession>();
		iocpCore->Register(session);
		string str = "user" + i;
		strcpy((char*)session->_name, str.c_str());
		sessionManager.Add(session);
		ASSERT_CRASH(session->Connect(SocketAddress(L"127.0.0.1", 7777)));
	}

	///* Single Connect */
	//ClientSessionRef session = make_shared<ClientSession>();
	//iocpCore->Register(session);
	//ASSERT_CRASH(session->Connect(SocketAddress(L"127.0.0.1", 7777)));

	/* workthread */
	vector<thread*> threads;
	for (int32 i = 0; i < threadCount; i++) {
		threads.push_back(new thread([&]() {
			while (true) {
				iocpCore->Dispatch();
			}
		}));
	}

	for (auto session: sessionManager.sessions) {
		// 1. 패킷 생성
		PacketLogInReq loginPkt;
		::memcpy(loginPkt.userName, session->_name, sizeof(session->_name));
		// 2. 헤더 부착
		PacketHeader header = MakeHeader<PacketLogInReq>(PacketId::Login_Req);
		OutputMemoryStreamRef inStream = make_shared<OutputMemoryStream>();
		inStream->Write(header.totalSize);
		inStream->Write(header.packetId);
		inStream->Write(loginPkt.userName, MAX_USER_NAME_SIZE);
		session->Send(inStream);
	}

	while (true) {
		string input  = "안녕하세요^^";
		// unsigned char 배열로 변환
		BYTE message[MAX_CHAT_SIZE];
		::memcpy(message, input.c_str(), input.size());
		for (auto session : sessionManager.sessions) {
			PacketHeader header = MakeHeader<PacketLobbyChatReq>(PacketId::Lobby_Chat_Req);
			PacketLobbyChatReq chatPkt;
			::memcpy(chatPkt.userName, session->_name, sizeof(session->_name));
			::memcpy(chatPkt.msg, message, input.size());
			OutputMemoryStreamRef stream = make_shared<OutputMemoryStream>();
			stream->Write(header.totalSize);
			stream->Write(header.packetId);
			stream->Write(chatPkt.userName, MAX_USER_NAME_SIZE);
			stream->Write(chatPkt.msg, MAX_CHAT_SIZE);
			session->Send(stream);
		}
		this_thread::sleep_for(250ms);
	}
	
	for (auto t : threads)
		t->join();

	SocketUtils::Clear();
}
