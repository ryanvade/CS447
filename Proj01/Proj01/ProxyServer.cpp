/*
* ProxyServer.cpp
*
* By Ryan Owens
*
* CS447
*
* Created September 8, 2017
*
*/

#include "ProxyServer.h"



ProxyServer::ProxyServer(int port, int webhost_port, int max_connections, int buff_size, std::string host, std::string web_host)
{
	// Get a new DATA object
	this->socketData = new WSADATA();
	// Set the default values
	this->port = port;
	this->max_connections = max_connections;
	this->max_buffer_size = buff_size;
	this->host = host;
	this->web_host = web_host;
	this->webhost_port = webhost_port;
}


ProxyServer::~ProxyServer()
{
	freeaddrinfo(this->server_addr);
	closesocket(this->server_socket);
	WSACleanup();
	delete(this->socketData);
	delete(this->server_addr);
	this->server_addr = NULL;
	this->socketData = NULL;
}

int ProxyServer::initializeWinSock()
{
	// initialize the WinSOCK data struct
	int result = WSAStartup(MAKEWORD(2, 2), this->socketData);
	// Check for errors
	if (result != 0)
	{
		std::cerr << "Unable to startup winsock " << result << std::endl;
		return 1;
	}
	return 0;
}

int ProxyServer::initializeServerSocket()
{
	struct addrinfo *socket_addr = this->getServerAdderInfo();
	std::string port_str = std::to_string(this->port);
	// get the Server Address Information
	int result = getaddrinfo(this->host.c_str(), port_str.c_str(), socket_addr, &this->server_addr);
	// check for errors
	if (result != 0)
	{
		std::cerr << "Unable to get server addr " << result;
		WSACleanup();
		return 1;
	}
	// Get the socket
	SOCKET soc = socket(this->server_addr->ai_family, this->server_addr->ai_socktype, this->server_addr->ai_protocol);
	// Check for errors
	if (soc == INVALID_SOCKET)
	{
		std::cerr << "Unable to initialize the server socket " << WSAGetLastError() << std::endl;
		freeaddrinfo(this->server_addr);
		WSACleanup();
		return 1;
	}
	// Save the Socket
	this->server_socket = soc;
	return 0;
}

int ProxyServer::initializeClientSocket()
{
	struct addrinfo *socket_addr = this->getClientAdderInfo();
	std::string port_str = std::to_string(this->webhost_port);
	// get the Server Address Information
	int result = getaddrinfo(this->web_host.c_str(), port_str.c_str(), socket_addr, &this->client_addr);
	// check for errors
	if (result != 0)
	{
		std::cerr << "Unable to get server addr " << result;
		WSACleanup();
		return 1;
	}
	// Get the socket
	SOCKET soc = socket(this->client_addr->ai_family, this->client_addr->ai_socktype, this->client_addr->ai_protocol);
	// Check for errors
	if (soc == INVALID_SOCKET)
	{
		std::cerr << "Unable to initialize the server socket " << WSAGetLastError() << std::endl;
		freeaddrinfo(this->server_addr);
		WSACleanup();
		return 1;
	}
	this->client_sockets.push_back(soc);
	this->client_count = this->client_sockets.size() - 1;
	return 0;
}


int ProxyServer::bindSocket()
{
	int result = bind(this->server_socket, this->server_addr->ai_addr, (int)this->server_addr->ai_addrlen);
	if (result == SOCKET_ERROR)
	{
		std::cerr << "bind failed with error " << WSAGetLastError() << std::endl;
		freeaddrinfo(this->server_addr);
		closesocket(this->server_socket);
		WSACleanup();
		return 1;
	}
	return 0;
}

int ProxyServer::listenOnSocket()
{
	if (listen(this->server_socket, this->max_connections) == SOCKET_ERROR)
	{
		std::cerr << "Listen failed with error " << WSAGetLastError() << std::endl;
		closesocket(this->server_socket);
		WSACleanup();
		return 1;
	}
	return 0;
}

int ProxyServer::acceptOnSocket()
{
	this->browser_addr = new sockaddr_in();
	memset(this->browser_addr, 0, sizeof(sockaddr_in));
	int size = sizeof(sockaddr_in);
	SOCKET soc = accept(this->server_socket, (sockaddr*)this->browser_addr, &size);
	if (soc == INVALID_SOCKET)
	{
		this->coutMutex.lock();
		std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
		this->coutMutex.unlock();
		closesocket(this->server_socket);
		WSACleanup();
		return 1;
	}
	this->coutMutex.lock();
	std::cout << "Connection from " << inet_ntoa(this->browser_addr->sin_addr) 
		<< " arrived (Port No = " << htons(this->browser_addr->sin_port) << ")\n";
	this->coutMutex.unlock();
	this->child_sockets.push_back(soc);
	this->child_count = this->child_sockets.size() - 1;
	return 0;
}

int ProxyServer::connectToWebServer(int client_count)
{
	int result = connect(this->client_sockets[client_count], this->client_addr->ai_addr, this->client_addr->ai_addrlen);
	if (result == SOCKET_ERROR)
	{
		this->coutMutex.lock();
		std::cerr << "Unable to connect to web host " << WSAGetLastError() << std::endl;
		this->coutMutex.unlock();
		return 1;
	}
	
	if (this->client_sockets[client_count] == INVALID_SOCKET)
	{
		this->coutMutex.lock();
		std::cerr << "Unable to connect to web server " << WSAGetLastError() << std::endl;
		this->coutMutex.unlock();
		WSACleanup();
		return 1;
	}
	return 0;
}

addrinfo * ProxyServer::getServerAdderInfo()
{
	struct addrinfo *socket_addr = new addrinfo();
	memset(socket_addr, 0, sizeof(*socket_addr));
	socket_addr->ai_family = AF_INET;
	socket_addr->ai_socktype = SOCK_STREAM;
	socket_addr->ai_protocol = 0;
	return socket_addr;
}

addrinfo * ProxyServer::getClientAdderInfo()
{
	struct addrinfo *socket_addr = new addrinfo;
	memset(socket_addr, 0, sizeof(*socket_addr));
	socket_addr->ai_family = AF_INET;
	socket_addr->ai_socktype = SOCK_STREAM;
	socket_addr->ai_protocol = IPPROTO_TCP;
	return socket_addr;
}

void ProxyServer::init(char* hazard_cs_01, char* hazard_cs_02, char* hazard_sc_01, char* hazard_sc_02)
{
	if (this->initializeWinSock() != 0) exit(1);
	std::cout << "WinSock Initialized" << std::endl;
	if (this->initializeServerSocket() != 0) exit(1);
	std::cout << "Server Socket Initialized" << std::endl;
	if (this->bindSocket() != 0) exit(1);
	std::cout << "Server Socket Binded to " << this->host << ":" << this->port << std::endl;
	if (this->listenOnSocket() != 0) exit(1);
	std::cout << "Server Listening for connections" << std::endl << std::endl;
	this->cs_hazards_01 = hazard_cs_01;
	this->cs_hazards_02 = hazard_cs_02;
	this->sc_hazards_01 = hazard_sc_01;
	this->sc_hazards_02 = hazard_sc_02;
}

int ProxyServer::clientServer(SOCKET child_socket, SOCKET client_socket)
{
	int result;
	char* buff = new char[this->max_buffer_size];
	do
	{
		result = recv(child_socket, buff, this->max_buffer_size, 0);
		this->coutMutex.lock();
		std::cout << "received " << result << " bytes from the client" << std::endl;
		this->coutMutex.unlock();
		if (result > 0)
		{
			this->coutMutex.lock();
			std::cout << "Sending " << result << " bytes to the web server" << std::endl;
			this->coutMutex.unlock();
			// send to the server
			int send_result = send(client_socket, buff, result, 0);
			if (send_result == SOCKET_ERROR)
			{
				this->coutMutex.lock();
				std::cerr << "Sending to web server failed " << WSAGetLastError() << std::endl;
				this->coutMutex.unlock();
				WSACleanup();
				return 1;
				//exit(1);
			}
			break;
		}
		else if (result == 0)
		{
			this->coutMutex.lock();
			std::cout << "connection to the client ended" << std::endl;
			this->coutMutex.unlock();
		}
		else
		{
			this->coutMutex.lock();
			std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
			this->coutMutex.unlock();
			return 1;
		}
	} while (result > 0);
	delete(buff);
	buff = NULL;
	this->coutMutex.lock();
	std::cout << "client server thread finished" << std::endl;
	this->coutMutex.unlock();
	return 0;
}

int ProxyServer::serverBrowser(SOCKET child_socket, SOCKET client_socket)
{
	int result;
	char* buff = new char[this->max_buffer_size];
	//Sleep(1000);
	do
	{
		result = recv(client_socket, buff, this->max_buffer_size, 0);
		this->coutMutex.lock();
		std::cout << "Received " << result << " bytes from the web server" << std::endl;
		this->coutMutex.unlock();
		if (result > 0)
		{
			this->coutMutex.lock();
			std::cout << "Sending " << result << " bytes from the web server to the client" << std::endl;
			this->coutMutex.unlock();
			// send to the client
			int client_send_result = send(child_socket, buff, result, 0);
			if (client_send_result == SOCKET_ERROR)
			{
				this->coutMutex.lock();
				std::cerr << "Sending to client failed " << WSAGetLastError() << std::endl;
				this->coutMutex.unlock();
				closesocket(this->client_sockets[client_count]);
				WSACleanup();
				//exit(1);
				return 1;
			}
		}
		else if (result == 0)
		{
			this->coutMutex.lock();
			std::cout << "connection to web server ended" << std::endl;
			this->coutMutex.unlock();
		}
		else
		{
			this->coutMutex.lock();
			std::cerr << "web server recv failed: " << WSAGetLastError() << std::endl;
			this->coutMutex.unlock();
		}
	} while (result  > 0);
	delete(buff);
	buff = NULL;	
	this->coutMutex.lock();
	std::cout << "server browser thread finished" << std::endl;
	this->coutMutex.unlock();

	closesocket(client_socket);
	closesocket(child_socket);
	return 0;
}

void ProxyServer::handleRequests()
{
	while (true)
	{
		// Accept the connection
		if (this->acceptOnSocket() != 0) exit(1);
		this->coutMutex.lock();
		std::cout << "Received a new request" << std::endl << std::endl;
		this->coutMutex.unlock();
		// Create client  (to web server) socket
		if (this->initializeClientSocket() != 0) exit(1);
		// connect client to webhost
		if (this->connectToWebServer(child_count) != 0) exit(1);
		// Get the newly created sockets
		SOCKET client_socket = this->client_sockets.back();
		SOCKET child_socket = this->child_sockets.back();
		// Spawn thread for client->server communication
		std::thread clientServerThread(&ProxyServer::clientServer, this, child_socket, client_socket);
		clientServerThread.detach();
		// Spawn thread for server->browser communication
		std::thread serverBrowserThread(&ProxyServer::serverBrowser, this, child_socket, client_socket);
		serverBrowserThread.detach();
		// end
	}
}
