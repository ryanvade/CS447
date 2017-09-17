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
#include <atomic>
#include <vector>
#include <mutex>

#pragma warning(disable : 4996)
#pragma comment(lib, "Ws2_32.lib")


class ProxyServer
{
public:
	ProxyServer(int port, int webhost_port, int max_connections, int buff_size, std::string host, std::string web_host);
	void init();
	void handleRequest();
	virtual ~ProxyServer();
private:
	int port, webhost_port, max_connections, max_buffer_size;
	std::string host, web_host;
	WSADATA *socketData = NULL;
	SOCKET server_socket;
	std::atomic<int> client_count = 0, child_count = -1;
	std::vector<SOCKET> client_sockets, child_sockets;
	struct addrinfo *child_connection = NULL;
	struct addrinfo *server_addr = NULL;
	struct addrinfo *client_addr = NULL;
	struct sockaddr_in *browser_addr = NULL;
	std::mutex coutMutex;

	int initializeWinSock();
	int initializeServerSocket();
	int initializeClientSocket();
	int bindSocket();
	int listenOnSocket();
	int acceptOnSocket();
	int connectToWebServer(int current);
	int clientServer(int child_count, int client_count);
	int serverBrowser(int child_count, int client_count);

	struct addrinfo* getServerAdderInfo();
	struct addrinfo* getClientAdderInfo();
	
};
