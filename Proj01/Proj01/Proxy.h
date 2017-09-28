/*
* ProxyServer.h
*
* By Ryan Owens
*
* CS447
*
* Created September 8, 2017
*
*/

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

#define NOTOK_404   "HTTP/1.0 404 Not Found\nContent-Type:text/html\n\n"
#define MESS_404    "<html><body><h1>FILE NOT FOUND</h1></body></html>"


class Proxy
{
public:
	Proxy(int port, int webhost_port, int max_connections, int buff_size, std::string host, std::string web_host);
	void init(char* hazard_cs_01, char* hazard_cs_02, char* hazard_sc_01, char* hazard_sc_02);
	void handleRequests();
	virtual ~Proxy();
private:
	int port, webhost_port, max_connections, max_buffer_size;
	std::string host, web_host;
	WSADATA *socketData = NULL;
	SOCKET server_socket;
	std::atomic<int> client_count = -1, child_count = -1;
	std::atomic<bool> hazards = false;
	std::vector<SOCKET> client_sockets, child_sockets;
	char *cs_hazards_01, *cs_hazards_02;
	char *sc_hazards_01, *sc_hazards_02;
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
	int clientServer(SOCKET child_socket, SOCKET client_socket);
	int serverBrowser(SOCKET child_socket, SOCKET client_socket);

	struct addrinfo* getServerAdderInfo();
	struct addrinfo* getClientAdderInfo();

	bool hasClientError(char* buff);
	bool hasServerError(char* buff);

	
};

