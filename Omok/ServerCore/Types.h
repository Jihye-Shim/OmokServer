#pragma once

using BYTE = unsigned char;
using WCHAR = wchar_t;
using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;

#include <memory>
using IocpCoreRef = std::shared_ptr<class IocpCore>;
using IocpObjectRef = std::shared_ptr<class IocpObject>;
using ListenerRef = std::shared_ptr<class Listener>;
using SessionRef = std::shared_ptr<class Session>;
using OutputMemoryStreamRef = std::shared_ptr<class OutputMemoryStream>;
using InputMemoryStreamRef = std::shared_ptr<class InputMemoryStream>;
using RoomRef = std::shared_ptr<class Room>;

// Macro
#define OUT

#define CRASH(cause)							\
{												\
	uint32* crash = nullptr;					\
	__analysis_assume(crash	!= nullptr);		\
	*crash = 0XDEADBEEF;						\
}
#define ASSERT_CRASH(expr)						\
{												\
	if(!(expr))									\
	{											\
		CRASH("ASSERT_CRASH");					\
		__analysis_assume(expr);				\
	}											\
}