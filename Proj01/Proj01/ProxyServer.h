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
#include <thread>

#pragma warning(disable : 4996)
#pragma comment(lib, "Ws2_32.lib")


class ProxyServer
{
public:
	ProxyServer(int port, int max_connections, int buff_size, std::string host, std::string web_host);
	void init();
	void handleRequest();
	virtual ~ProxyServer();
private:
	int port, max_connections, max_buffer_size;
	std::string host, web_host;
	WSADATA *socketData = NULL;
	SOCKET server_socket, client_socket, child_socket;
	struct addrinfo *child_connection;
	struct addrinfo *server_addr = NULL;
	struct sockaddr_in *client_addr;
	int initializeWinSock();
	int initializeServerSocket();
	int initializeClientSocket();
	int bindSocket();
	int listenOnSocket();
	int acceptOnSocket();
	int connectToWebServer();
	void clientServer();
	void serverBrowser();

	struct addrinfo* getServerAdderInfo();
	struct addrinfo* getClientAdderInfo();
	
};

