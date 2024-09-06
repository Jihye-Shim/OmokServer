#include "pch.h"
#include "Session.h"
#include "SocketUtils.h"
IocpEvent::IocpEvent(EventType eventType) : _eventType(eventType)
{
	OVERLAPPED::hEvent = 0;
	OVERLAPPED::Internal = 0;
	OVERLAPPED::InternalHigh = 0;
	OVERLAPPED::Offset = 0;
	OVERLAPPED::OffsetHigh = 0;
}

Session::Session()
{
	//_socket = SocketUtils::CreateSocket(); -> acceptEx 사용시
}

Session::~Session()
{
}

void Session::SetSocketAddress(SocketAddress sAddress)
{
	_sockAddress = sAddress;
}

void Session::Dispatch(IocpEvent* iocpEvent, int32 numOfByte)
{
	//Event로 session 꺼내다 쓰기
	switch (iocpEvent->_eventType) {
	case EventType::Send:
		iocpEvent->owner->ProcessSend();
		break;
	case EventType::Recv:
		iocpEvent->owner->ProcessRecv();
		break;
	default:
		break;
	}
}

void Session::RegisterRecv()
{
	_recvEvent.owner = shared_from_this();

	WSABUF wsa;
	wsa.buf = reinterpret_cast<char*>(_recvBuffer);
	wsa.len = recvSize;
	DWORD numOfBytes;
	DWORD flag = 0;
	if (SOCKET_ERROR == ::WSARecv(_socket, &wsa, 1, OUT & numOfBytes, OUT & flag, &_recvEvent, nullptr))
	{
		int32 errCode = ::WSAGetLastError();
		if (errCode != WSA_IO_PENDING) {
			cout << "Recv Handle Error : " << errCode << endl;
			_recvEvent.owner = nullptr;
		}
	}
}

void Session::ProcessRecv()
{
	_recvEvent.owner = nullptr;

	OnRecv();
	::memset(_recvBuffer, '\0', sizeof(_recvBuffer));
	RegisterRecv();
}

void Session::Send(BYTE* buffer)
{
	::memcpy(_sendBuffer, buffer,sizeof(buffer));
	RegisterSend();
}

void Session::RegisterSend()
{
	_sendEvent.owner = shared_from_this();

	WSABUF wsa;
	wsa.buf = reinterpret_cast<char*>(_sendBuffer);
	wsa.len = recvSize;
	DWORD numOfBytes = 0;
	if (SOCKET_ERROR == ::WSASend(_socket, &wsa, 1, OUT &numOfBytes, 0, &_sendEvent, nullptr))
	{
		int32 errCode = ::WSAGetLastError();
		if (errCode != WSA_IO_PENDING) {
			cout << "Send Handle Error : " << errCode << endl;
			_sendEvent.owner = nullptr;
		}
	}
}

void Session::ProcessSend()
{
	_sendEvent.owner = nullptr;

	OnSend();

//	RegisterSend();
}
