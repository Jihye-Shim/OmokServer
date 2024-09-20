#include "pch.h"
#include "Lock.h"

LockGuard::LockGuard(mutex& mutex) : _mutex(mutex)
{
	_mutex.lock();
}

LockGuard::~LockGuard()
{
	_mutex.unlock();
}
