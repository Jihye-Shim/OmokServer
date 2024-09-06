#pragma once
#include "SocketAddress.h"
class SocketUtils
{

public:
	static void Init();
	static void Clear();

	static SOCKET CreateSocket();
	static void CloseSocket(SOCKET& socket);

	static bool Bind(SOCKET socket, SocketAddress sockAddr);
	static bool BindAnyAddress(SOCKET socket, uint16 port);
	static bool Listen(SOCKET socket, int32 backlog = SOMAXCONN);
	static bool Accept(SOCKET listenSocket, SocketAddress* sockAddr);
	static bool Connect(SOCKET socket, SocketAddress* sockAddr);
};

