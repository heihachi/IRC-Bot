/*
    Header file for configurations will be replaced later for file loading.
*/

#ifndef config_h
#define config_h
#include <string>
#include <cstdlib>
#include <cstdio>
#ifdef _LINUX
#include <sys/sockets.h>
#elseif _WIN32
#include <winsock.h>
#endif // OS based includes

using namespace std;

bool configuration(string[8][2]);

#endif // config
