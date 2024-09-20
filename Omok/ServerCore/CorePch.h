#pragma once

#include "Types.h"

#include <thread>
#include <atomic>
#include <mutex>

#include <vector>
#include <array>
#include <list>
#include <map>
#include <set>
#include <tuple>

#include <iostream>
using namespace std;

#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include "SocketUtils.h"
#include "Lock.h"