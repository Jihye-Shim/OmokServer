#pragma once
using SocketAddressRef = shared_ptr<class SocketAddress>;

class SocketAddress
{
public:
	SocketAddress() = default;
	SocketAddress(SOCKADDR_IN address);
	SocketAddress(wstring ip, int32 port);

	SOCKADDR_IN& GetSockAddr() { return _sockAddr; }
private:
	SOCKADDR_IN _sockAddr = {};
};

