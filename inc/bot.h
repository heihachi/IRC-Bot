/*
    Header file for configurations will be replaced later for file loading.
*/

#ifndef bot_h
#define bot_h

#include "config.h"
#include <winsock2.h>
#define _WIN32_WINNT 0x501
#include <ws2tcpip.h>
#include <stdio.h>
#include <windows.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#pragma comment (lib, "wsock32.lib")

bool bot(string[8][2]);
int recv_all(int, bool, size_t, int);

#endif // bot
