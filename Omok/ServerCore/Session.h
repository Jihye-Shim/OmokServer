#pragma once
#include "SocketAddress.h"
#include <atomic>
enum class EventType {
	Send,
	Recv
};
const int BUFSIZE = 1000;
class Session;
class IocpEvent : public OVERLAPPED {
public:
	IocpEvent(EventType eventType);
public:
	EventType _eventType;
	shared_ptr<Session> owner = nullptr;
};
class SendEvent : public IocpEvent{
public:
	SendEvent() : IocpEvent(EventType::Send) {}
public:
};
class RecvEvent : public IocpEvent {
public:
	RecvEvent(): IocpEvent(EventType::Recv) {}
};

class Session : public enable_shared_from_this<Session>
{
public:
	Session();
	~Session();

public:
	void SetSocket(SOCKET socket) { _socket = socket; }
	void SetSocketAddress(SocketAddress sAddress);

	HANDLE GetHandle() { return reinterpret_cast<HANDLE>(_socket); }
	void Dispatch(IocpEvent* iocpEvent, int32 numOfByte);

private:
	virtual void OnSend() abstract;
	virtual void OnRecv() abstract;

public:
	void RegisterRecv();
	void ProcessRecv();

	void Send(BYTE* buffer);
	void RegisterSend();
	void ProcessSend();

public:
	BYTE _recvBuffer[1000];
	BYTE _sendBuffer[1000];
	int32 recvSize = BUFSIZE;
	SOCKET _socket = INVALID_SOCKET;
	char* _name;
//	atomic<bool> _registerRecv;

private:

	SocketAddress _sockAddress = {};
	RecvEvent _recvEvent;
	SendEvent _sendEvent;
};

