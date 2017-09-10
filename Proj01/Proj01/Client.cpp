#include "Client.h"



Client::Client(WSADATA *winsock_data)
{
	this->winsock_data = winsock_data;
}


Client::~Client()
{
}


int Client::connectToWebHost(std::string web_host)
{
	if (this->initializeClientSocket() != 0) exit(1);

	int result = connect(*this->client_socket, this->server_addr->ai_addr, (int)this->server_addr->ai_addrlen);
	if (result == SOCKET_ERROR)
	{
		closesocket(*this->client_socket);
		this->client_socket = NULL;
		return 1;
	}
	// Spawn Client to Server Thread
	// Spawn Server to client Thread
	return 0;
}

addrinfo * Client::getServerAdderInfo()
{
	struct addrinfo *socket_addr = new addrinfo;
	ZeroMemory(socket_addr, sizeof(socket_addr));
	socket_addr->ai_family = AF_INET;
	socket_addr->ai_socktype = SOCK_STREAM;
	socket_addr->ai_protocol = 0;
	return socket_addr;
}


int Client::initializeClientSocket()
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
	if (setsockopt(soc, SOL_SOCKET, SO_LINGER, (LPSTR)&this->ling, sizeof(this->ling)))
	{
		std::cerr << "Could not attach linger to socket" << WSAGetLastError() << std::endl;
		freeaddrinfo(this->server_addr);
		closesocket(*this->client_socket);
		WSACleanup();
		return 1;
	}
	// Save the Socket
	this->client_socket = &soc;
	return 0;
}
