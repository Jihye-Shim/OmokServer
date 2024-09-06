#include "pch.h"
#include "SocketUtils.h"

void SocketUtils::Init()
{
	WSAData wsaData;
	ASSERT_CRASH(::WSAStartup(MAKEWORD(2, 2), OUT & wsaData) == 0);
}

void SocketUtils::Clear()
{
	::WSACleanup();
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

bool SocketUtils::Accept(SOCKET listenSocket, SocketAddress* sockAddr)
{
	int32 addLen = sizeof(SOCKADDR_IN);
	return SOCKET_ERROR != ::accept(listenSocket, reinterpret_cast<SOCKADDR*>(&sockAddr->GetSockAddr()), &addLen);
}

bool SocketUtils::Connect(SOCKET socket, SocketAddress* sockAddr)
{
	int32 addLen = sizeof(SOCKADDR_IN);
	return SOCKET_ERROR != ::connect(socket, reinterpret_cast<SOCKADDR*>(&sockAddr->GetSockAddr()), addLen);
}

