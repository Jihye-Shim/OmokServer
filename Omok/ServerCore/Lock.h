#pragma once

class LockGuard
{
	LockGuard(mutex& mutex);
	~LockGuard();
private:
	mutex& _mutex;
};
//
//#define USE_LOCK 	mutex mtx;
//#define LOCK		LockGuard();
#define USE_LOCK	mutex _mutex;
#define LOCKGUARD		lock_guard<mutex> lock_guard(_mutex);
