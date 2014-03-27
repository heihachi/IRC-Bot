/*
    Header file for configurations will be replaced later for file loading.
*/

#ifndef config_h
#define config_h
#include <string>
#include <iostream>
#include <cstdlib>
#include <stdlib.h>
#include <cstdio>
#include <stdio.h>
#include <winsock2.h>
#define _WIN32_WINNT 0x501
#include <ws2tcpip.h>
#include <stdio.h>
#include <windows.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
using namespace std;

bool configuration(string[8][2]);

#endif // config
