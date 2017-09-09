#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")


class ProxyServer
{
public:
	ProxyServer(int port);
	virtual ~ProxyServer();
private:
	int port;
	WSADATA *socketData = NULL;
	SOCKET *server_socket = NULL;
	int initializeWinSock();
	int initializeServerSocket();
};

