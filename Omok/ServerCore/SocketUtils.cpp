#include "pch.h"
#include "SocketUtils.h"

LPFN_CONNECTEX SocketUtils::ConnectEx = nullptr;
LPFN_DISCONNECTEX SocketUtils::DisconnectEx = nullptr;
LPFN_ACCEPTEX SocketUtils::AcceptEx = nullptr;

void SocketUtils::Init()
{
	WSAData wsaData;
	ASSERT_CRASH(::WSAStartup(MAKEWORD(2, 2), OUT & wsaData) == 0);

	SOCKET DummySocket = CreateSocket();
	ASSERT_CRASH(BindWindowsFunction(DummySocket, WSAID_CONNECTEX, reinterpret_cast<LPVOID*>(&ConnectEx)));
	ASSERT_CRASH(BindWindowsFunction(DummySocket, WSAID_DISCONNECTEX, reinterpret_cast<LPVOID*>(&DisconnectEx)));
	ASSERT_CRASH(BindWindowsFunction(DummySocket, WSAID_ACCEPTEX, reinterpret_cast<LPVOID*>(&AcceptEx)));
	CloseSocket(DummySocket);
}

void SocketUtils::Clear()
{
	::WSACleanup();
}

bool SocketUtils::BindWindowsFunction(SOCKET dummySocket, GUID guid, LPVOID* fn)
{
	DWORD dwBytes = 0;
	return SOCKET_ERROR != ::WSAIoctl(dummySocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), fn, sizeof(fn), OUT &dwBytes, nullptr, nullptr);
}

SOCKET SocketUtils::CreateSocket()
{
	return ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
}

void SocketUtils::CloseSocket(SOCKET& socket)
{
	if (socket != INVALID_SOCKET)
		::closesocket(socket);
	socket = INVALID_SOCKET;
}

bool SocketUtils::SetLinger(SOCKET socket, uint16 onoff, uint16 linger)
{
	LINGER option;
	option.l_onoff = onoff;
	option.l_linger = linger;
	return SOCKET_ERROR != ::setsockopt(socket, SOL_SOCKET, SO_LINGER, (const char*)&option, sizeof(option));
}

bool SocketUtils::SetReuseAddr(SOCKET socket, bool flag)
{
	return SOCKET_ERROR != ::setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&flag, sizeof(bool));
}

bool SocketUtils::SetRecvBufferSize(SOCKET socket, int size)
{
	return SOCKET_ERROR != ::setsockopt(socket, SOL_SOCKET, SO_RCVBUF, (const char*)&size, sizeof(int));
}

bool SocketUtils::SetSendBufferSize(SOCKET socket, int size)
{
	return SOCKET_ERROR != ::setsockopt(socket, SOL_SOCKET, SO_SNDBUF, (const char*)&size, sizeof(int));
}

bool SocketUtils::SetTcpNoDelay(SOCKET socket, bool flag)
{
	return SOCKET_ERROR != ::setsockopt(socket, SOL_SOCKET, TCP_NODELAY, (const char*)&flag, sizeof(bool));
}

bool SocketUtils::SetKeepAlive(SOCKET socket, bool flag)
{
	return SOCKET_ERROR != ::setsockopt(socket, SOL_SOCKET, SO_KEEPALIVE, (const char*)&flag, sizeof(bool));
}

bool SocketUtils::SetUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket)
{
	return SOCKET_ERROR != ::setsockopt(socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (const char*)&listenSocket, sizeof(SOCKET));
}

bool SocketUtils::Bind(SOCKET socket, SocketAddress sockAddr)
{
	return SOCKET_ERROR != ::bind(socket, (SOCKADDR*)(&sockAddr.GetSockAddr()), sizeof(SOCKADDR_IN));
}

bool SocketUtils::BindAnyAddress(SOCKET socket, uint16 port)
{
	SOCKADDR_IN sockAddr;
	::memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	sockAddr.sin_port = ::htons(port);
//	cout << "address : " << sockAddr.sin_addr.s_addr << endl;
	return SOCKET_ERROR != ::bind(socket, (SOCKADDR*)&sockAddr, sizeof(sockAddr));
}

bool SocketUtils::Listen(SOCKET socket, int32 backlog)
{
	return SOCKET_ERROR != ::listen(socket, backlog);
}

