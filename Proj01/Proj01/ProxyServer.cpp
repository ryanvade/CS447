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
	/*LINGER ling;
	ling.l_linger = 0;
	ling.l_onoff = 1;
	if (setsockopt(this->client_socket, SOL_SOCKET, SO_LINGER, (LPTSTR)&ling, sizeof(ling)) < 0)
	{
		std::cerr << "Unable to add LINGER to socket" << std::endl;
		closesocket(this->client_socket);
		return 1;
	}*/
	// Save the Socket
	this->client_socket = soc;
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
		std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
		closesocket(this->server_socket);
		WSACleanup();
		return 1;
	}
	std::cout << "Connection from " << inet_ntoa(this->browser_addr->sin_addr) 
		<< " arrived (Port No = " << htons(this->browser_addr->sin_port) << ")\n";	
	this->child_socket = soc;
	return 0;
}

int ProxyServer::connectToWebServer()
{
	int result = connect(this->client_socket, this->client_addr->ai_addr, this->client_addr->ai_addrlen);
	if (result == SOCKET_ERROR)
	{
		std::cerr << "Unable to connect to web host " << WSAGetLastError() << std::endl;
		closesocket(this->client_socket);
		return 1;
	}
	
	if (this->client_socket == INVALID_SOCKET)
	{
		std::cerr << "Unable to connect to web server " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}
	return 0;
}

void ProxyServer::clientServer()
{
	int result;
	char* buff = new char[this->max_buffer_size];
	do
	{
		result = recv(this->child_socket, buff, this->max_buffer_size, 0);
		std::cout << "received " << result << " bytes from the client" << std::endl;
		if (result > 0)
		{
			std::cout << "\t\t\t\t\tSending " << result << " bytes to the web server" << std::endl;
			// send to the server
			int send_result = send(this->client_socket, buff, result, 0);
			if (send_result == SOCKET_ERROR)
			{
				std::cerr << "Sending to web server failed " << WSAGetLastError() << std::endl;
				closesocket(this->client_socket);
				WSACleanup();
				break;
			}
		}
		else if (result == 0)
		{
			std::cout << "connection to the client ended" << std::endl;
		}
		else
		{
			std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
		}
	} while (result > 0);
	delete(buff);
	buff = NULL;
}

void ProxyServer::serverBrowser()
{
	int result;
	char* buff = new char[this->max_buffer_size];
	Sleep(1000);
	do
	{
		result = recv(this->client_socket, buff, this->max_buffer_size, 0);
		std::cout << "Received " << result << " bytes from the web server" << std::endl;
		if (result > 0)
		{
			std::cout << "\t\t\t\t\t\tSending " << result << " bytes from the web server to the client" << std::endl;
			// send to the client
			int client_send_result = send(this->child_socket, buff, result, 0);
			if (client_send_result == SOCKET_ERROR)
			{
				std::cerr << "Sending to client failed " << WSAGetLastError() << std::endl;
				closesocket(this->client_socket);
				WSACleanup();
				break;
			}
		}
		else if (result == 0)
		{
			std::cout << "connection to web server ended" << std::endl;
		}
		else
		{
			std::cerr << "web server recv failed: " << WSAGetLastError() << std::endl;
		}
	} while (result  > 0);
	delete(buff);
	buff = NULL;
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
	socket_addr->ai_protocol = 0;
	return socket_addr;
}

void ProxyServer::init()
{
	if (this->initializeWinSock() != 0) exit(1);
	std::cout << "WinSock Initialized" << std::endl;
	if (this->initializeServerSocket() != 0) exit(1);
	std::cout << "Server Socket Initialized" << std::endl;
	if (this->bindSocket() != 0) exit(1);
	std::cout << "Server Socket Binded to " << this->host << ":" << this->port << std::endl;
	if (this->listenOnSocket() != 0) exit(1);
	std::cout << "Server Listening for connections" << std::endl << std::endl;
}

void ProxyServer::handleRequest()
{
	// Accept the connection
	if (this->acceptOnSocket() != 0) exit(1);
	std::cout << "Received a new request" << std::endl << std::endl;
	// Create client  (to web server) socket
	if (this->initializeClientSocket() != 0) exit(1);
	// connect client to webhost
	if (this->connectToWebServer() != 0) exit(1);
	// Spawn thread for client->server communication
	std::thread clientServerThread (&ProxyServer::clientServer, this);
	// Spawn thread for server->browser communication
	std::thread serverBrowserThread (&ProxyServer::serverBrowser, this);
	serverBrowserThread.join(); // pause until thread is finished
	clientServerThread.join();
	std::cout << "Both threads have finished " << std::endl;
	closesocket(this->client_socket);
	closesocket(this->child_socket);

	// end
}
