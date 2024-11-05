#pragma once
#pragma warning(disable:4996)
using SocketAddressRef = shared_ptr<class SocketAddress>;

class SocketAddress
{
public:
	SocketAddress() = default;
	SocketAddress(SOCKADDR_IN address);
	SocketAddress(wstring ip, int32 port);

	SOCKADDR_IN& GetSockAddr() { return _sockAddr; }
	string GetAdddr() {
		string str = inet_ntoa(_sockAddr.sin_addr);
		str.append(":");
		str.append(to_string(ntohs(_sockAddr.sin_port)));
		return str;
	}
private:
	SOCKADDR_IN _sockAddr = {};
};

