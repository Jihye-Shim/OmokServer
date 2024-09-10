#pragma once
#include "IocpCore.h"
#include "IocpEvent.h"
#include <functional>

using SessionFactory = function<SessionRef(void)>;

class Listener : public IocpObject
{
public:
	Listener(SocketAddress sockAddr, SessionFactory sessionFactory);
	~Listener();
	
	/* 인터페이스 */
	virtual HANDLE GetHandle() override { return reinterpret_cast<HANDLE>(_socket); }
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;
public:
	/* Accept 기능*/
	bool StartAccept();
	void CloseSocket();

	void RegisterAccept(AcceptEvent* acceptEvent);
	void ProcessAccept(AcceptEvent* acceptEvent);

private:
	vector<AcceptEvent*> _acceptEvents;

private:
	SOCKET _socket;
	SocketAddress _sockAddress; // my
	SessionFactory _sessionFactory;
};

