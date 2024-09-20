#pragma once
#include "IocpCore.h"
#include "IocpEvent.h"

class Session : public IocpObject
{
	enum {
		BUF_SIZE = 1000
	};
public:
	Session();
	~Session();
public:
	/* 정보 */
	void SetSocketAddress(SocketAddress address) { _sockAddress = address; }
	SocketAddress GetAddress() { return _sockAddress; }
	SOCKET GetSocket() { return _socket; }
	bool IsConnected() { return _connected; }

public:
	/* 인터페이스*/
	virtual HANDLE GetHandle() override { return reinterpret_cast<HANDLE>(_socket); }
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;
public:
	/* 외부 사용 */
	bool Connect(SocketAddress targetAddress);
	void Disconnect(const WCHAR* cause);
	void Send(BYTE* sendBuffer);

public:
	/* 컨텐츠 오버로드 */
	virtual void OnConnected() {};
	virtual void OnDisconnected() {};
	virtual void OnSend(int32 numOfBytes) {};
	virtual void OnRecv(int32 numOfBytes) {};

	/* Conenct, Send, Recv */
	bool RegisterConnect(SocketAddress targetAddress);
	void RegisterDisconnect();
	void RegisterRecv();
	void RegisterSend(SendEvent* sendEvent);

	void ProcessConnect();
	void ProcessDisconnect();
	void ProcessRecv(int32 numOfBytes);
	void ProcessSend(SendEvent* sendEvent, int32 numOfBytes);

	void HandleError(int32 errCode);
public:
	//USELOCK
	
	BYTE _recvBuffer[BUF_SIZE];
	BYTE _sendBuffer[BUF_SIZE];

private:
	ConnectEvent _connectEvent;
	DisconnectEvent _disconnectEvent;
	RecvEvent _recvEvent;
//	SendEvent _sendEvent;

private:
	SOCKET _socket;
	SocketAddress _sockAddress; // Client: my(bindAnyAddress) Server: target(getpeername)
	
	USE_LOCK
//	BYTE recvBuffer[1000];
//	BYTE sendBuffer[1000];
	atomic<bool> _connected = false;
};

