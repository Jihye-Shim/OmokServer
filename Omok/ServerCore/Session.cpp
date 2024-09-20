#include "pch.h"
#include "Session.h"

Session::Session()
{
	_socket = SocketUtils::CreateSocket();
	ASSERT_CRASH(_socket != INVALID_SOCKET);
}
Session::~Session()
{
	SocketUtils::CloseSocket(_socket);
}

void Session::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	switch (iocpEvent->_eventType) {
	case EventType::Connect:
		ProcessConnect();
		break;
	case EventType::Disconnect:
		ProcessDisconnect();
		break;
	case EventType::Recv:
		ProcessRecv(numOfBytes);
		break;
	case EventType::Send:
		ProcessSend(static_cast<SendEvent*>(iocpEvent), numOfBytes);
		break;
	default:
		break;
	}
}

bool Session::Connect(SocketAddress targetAddress)
{
	return RegisterConnect(targetAddress);
}

void Session::Disconnect(const WCHAR* cause)
{
	//_connect ªÛ≈¬ ∫Ø∞Ê
	if (_connected.exchange(false) == false)
		return;
	
	wcout << "Disconnect: " << cause << endl;
	RegisterDisconnect();
}

void Session::Send(BYTE* sendBuffer)
{
	if (false == IsConnected())
		return;
	SendEvent* sendEvent = new SendEvent();
	sendEvent->owner = shared_from_this();
	::memcpy(_sendBuffer, sendBuffer, strlen((char*)sendBuffer) + 1);

	LOCKGUARD;
	RegisterSend(sendEvent);
}

bool Session::RegisterConnect(SocketAddress targetAddress)
{
	if (IsConnected())
		return false;
	// _socket √ ±‚»≠
	if (false == SocketUtils::SetReuseAddr(_socket, true))
		return false;
	if (false == SocketUtils::SetKeepAlive(_socket, true))
		return false;
	if (false == SocketUtils::BindAnyAddress(_socket, 0))
		return false;

	//_connectEvent √ ±‚»≠
	_connectEvent.Init();
	_connectEvent.owner = shared_from_this();

	SOCKADDR_IN addr = targetAddress.GetSockAddr(); // targetAddress
	DWORD numOfBytes;
	if(SOCKET_ERROR == SocketUtils::ConnectEx(_socket, reinterpret_cast<SOCKADDR*>(&addr), sizeof(addr),
		nullptr, 0, OUT & numOfBytes, &_connectEvent)){
		int32 errCode = WSAGetLastError();
		if (errCode != WSA_IO_PENDING) {
			HandleError(errCode);
			_connectEvent.owner = nullptr;
			return false;
		}
	}
	return true;
}

void Session::RegisterDisconnect()
{
	_disconnectEvent.Init();
	_disconnectEvent.owner = shared_from_this();

	if (SOCKET_ERROR == SocketUtils::DisconnectEx(_socket, &_disconnectEvent, TF_REUSE_SOCKET, 0)) {
		int32 errCode = WSAGetLastError();
		if (errCode != WSA_IO_PENDING) {
			_disconnectEvent.owner = nullptr;
			return;
		}
	}
}

void Session::RegisterRecv()
{
	if (false == IsConnected())
		return;

	_recvEvent.Init();
	_recvEvent.owner = shared_from_this();
	DWORD numOfBytes = 0;
	DWORD flag = 0;
	WSABUF wsa;
	wsa.buf = reinterpret_cast<char*>(_recvBuffer);
	wsa.len = BUF_SIZE;
	if (SOCKET_ERROR == ::WSARecv(_socket, &wsa, 1, OUT &numOfBytes, OUT &flag, &_recvEvent, nullptr)) {
		int32 errCode = WSAGetLastError();
		if (errCode != WSA_IO_PENDING) {
			HandleError(errCode);
			_recvEvent.owner = nullptr;
			return;
		}
	}
}

void Session::RegisterSend(SendEvent* sendEvent)
{
	if (false == IsConnected())
		return;

	DWORD numOfBytes = 0;
	WSABUF wsa;
	wsa.buf = reinterpret_cast<char*>(_sendBuffer);
	wsa.len = BUF_SIZE;
	if (SOCKET_ERROR == ::WSASend(_socket, &wsa, 1, OUT &numOfBytes, 0, sendEvent, nullptr)) {
		int32 errCode = WSAGetLastError();
		if (errCode != WSA_IO_PENDING) {
			HandleError(errCode);
			sendEvent->owner = nullptr;
			::memset(_sendBuffer, 0, BUF_SIZE);
			return;
		}
	}
}

void Session::ProcessConnect()
{
	_connectEvent.owner = nullptr;
	_connected.store(true);

	OnConnected();

	RegisterRecv(); // recv start
}

void Session::ProcessDisconnect()
{
	_disconnectEvent.owner = nullptr;
	OnDisconnected();
}

void Session::ProcessRecv(int32 numOfBytes)
{
	_recvEvent.owner = nullptr;
	if (numOfBytes == 0) { // ø¨∞· ≤˜±Ë
		Disconnect(L"Recv 0");
		return;
	}
	OnRecv(numOfBytes);
	//recvbuffer clean
	RegisterRecv();
}

void Session::ProcessSend(SendEvent* sendEvent, int32 numOfBytes)
{
	sendEvent->owner = nullptr;
	if (numOfBytes == 0) { // ø¨∞· ≤˜±Ë
		Disconnect(L"Send 0");
		return;
	}
	// send buffer clear
	OnSend(numOfBytes);
}


void Session::HandleError(int32 errCode)
{
	switch (errCode) {
	case WSAECONNRESET:
	case WSAECONNABORTED:
		Disconnect(L"Invalid Connect");
		break;
	default:
		cout << "Handle Error : " << errCode << endl;
		break;
	}
}
