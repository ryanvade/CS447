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
#include <string>

#pragma comment(lib, "Ws2_32.lib")

class Client
{
public:
	Client(WSADATA *winsock_data);
	virtual ~Client();
	int connectToWebHost(std::string web_host);

private:
	int port = 80;
	std::string host;
	struct addrinfo *server_addr = NULL;
	WSADATA *winsock_data;
	SOCKET *client_socket;
	LINGER ling;
	int initializeClientSocket();
	struct addrinfo* getServerAdderInfo();
};

