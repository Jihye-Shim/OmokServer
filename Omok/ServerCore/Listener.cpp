#include "pch.h"
#include "Listener.h"
#include "Session.h"

Listener::Listener(SocketAddress sockAddr, IocpCoreRef iocpCore, SessionFactory sessionFactory) 
	: _sockAddress(sockAddr), _iocpCore(iocpCore), _sessionFactory(sessionFactory)
{
	_socket = SocketUtils::CreateSocket();
	ASSERT_CRASH(_socket != INVALID_SOCKET);
}

Listener::~Listener()
{
	SocketUtils::CloseSocket(_socket);
	for (AcceptEvent* acceptEvent : _acceptEvents) {
		//TODO
		delete(acceptEvent);
	}
}

void Listener::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	ASSERT_CRASH(iocpEvent->_eventType == EventType::Accept);
	AcceptEvent* acceptEvent = static_cast<AcceptEvent*>(iocpEvent);
	ProcessAccept(acceptEvent);
}

bool Listener::StartAccept()
{
	// Socket 초기화 및 연결 준비
	if (!SocketUtils::SetReuseAddr(_socket, true))
		return false;
	if (!SocketUtils::SetLinger(_socket, 0, 0))
		return false;
	if (!SocketUtils::Bind(_socket, _sockAddress))
		return false;
	if (!SocketUtils::Listen(_socket))
		return false;
	
	//AcceptEvent 관리
	int32 acceptCount = 100;
	for (int32 i = 0; i < acceptCount; i++) {
		AcceptEvent* acceptEvent = new AcceptEvent();
		acceptEvent->owner = shared_from_this();
		_acceptEvents.push_back(acceptEvent);
		RegisterAccept(acceptEvent);
	}
	return true;
}

void Listener::CloseSocket()
{
	SocketUtils::CloseSocket(_socket);
}

void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
	SessionRef session = _sessionFactory();
	_iocpCore->Register(session);
	acceptEvent->Init();
	acceptEvent->session = session;
	DWORD dwBytes;
	if (!SocketUtils::AcceptEx(_socket, session->GetSocket(), session->_recvBuffer, 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, OUT & dwBytes, acceptEvent))
	{
		int32 errCode = WSAGetLastError();
		if(errCode != WSA_IO_PENDING)
			RegisterAccept(acceptEvent);
	}
}

void Listener::ProcessAccept(AcceptEvent* acceptEvent)
{
	SessionRef session = acceptEvent->session;
	
	if (!SocketUtils::SetUpdateAcceptSocket(session->GetSocket(), _socket)) {
		RegisterAccept(acceptEvent);
		return;
	}

	SOCKADDR_IN addr;
	int32 addrLen = sizeof(SOCKADDR_IN);
	if (SOCKET_ERROR == ::getpeername(session->GetSocket(), OUT reinterpret_cast<SOCKADDR*>(&addr), &addrLen)) {
		RegisterAccept(acceptEvent);
		return;
	}

	session->SetSocketAddress(SocketAddress(addr));
	session->ProcessConnect();
	RegisterAccept(acceptEvent);
}
