#pragma once

enum class EventType {
	Send,
	Recv,
	Connnect,
	Disconnect,
	Accept
};

class IocpEvent : public OVERLAPPED {
public:
	IocpEvent(EventType eventType);
	void Init();
public:
	EventType _eventType;
	IocpObjectRef owner = nullptr;
};

class SendEvent : public IocpEvent {
public:
	SendEvent() : IocpEvent(EventType::Send) {}
public:
};

class RecvEvent : public IocpEvent {
public:
	RecvEvent() : IocpEvent(EventType::Recv) {}
};

class ConnectEvent : public IocpEvent {
public:
	ConnectEvent() : IocpEvent(EventType::Connnect) {}
};

class DisconnectEvent : public IocpEvent {
public:
	DisconnectEvent() : IocpEvent(EventType::Disconnect) {}
};

class AcceptEvent : public IocpEvent {
public:
	AcceptEvent() : IocpEvent(EventType::Accept) {}

	SessionRef session = nullptr;
};