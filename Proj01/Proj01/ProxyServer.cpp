#include "ProxyServer.h"



ProxyServer::ProxyServer(int port)
{
	// Get a new DATA object
	this->socketData = new WSADATA();
	// Set the default values
	this->port = port;
}


ProxyServer::~ProxyServer()
{
	delete(this->socketData);
	delete(this->server_socket);
	this->socketData = NULL;
	this->server_socket = NULL;
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
	// Get the socket
	SOCKET soc = socket(AF_INET, SOCK_STREAM, 0);
	// Check for errors
	if (soc == INVALID_SOCKET)
	{
		std::cerr << "Unable to initialize the server socket " << WSAGetLastError() << std::endl;
		return 1;
	}
	// Save the Socket
	this->server_socket = &soc;
	return 0;
}
