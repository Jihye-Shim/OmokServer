#include "pch.h"
#include "SocketAddress.h"

SocketAddress::SocketAddress(SOCKADDR_IN address) : _sockAddr(address)
{
}

SocketAddress::SocketAddress(wstring ip, int32 port)
{
	::memset(&_sockAddr, 0, sizeof(_sockAddr));
	_sockAddr.sin_family = AF_INET;
	::InetPtonW(AF_INET, ip.c_str(), &_sockAddr.sin_addr);
	_sockAddr.sin_port = ::htons(port);
}
