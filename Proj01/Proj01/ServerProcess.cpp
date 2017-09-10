#include "ProxyServer.h"

#define SERVER_PORT 9080
#define MAXCONNECTIONS 3
#define BUFFER_SIZE 100
#define HOST "localhost"
#define WEBHOST "172.217.8.206"
int main()
{
	// Get a new Server
	ProxyServer *server = new ProxyServer(SERVER_PORT, MAXCONNECTIONS, BUFFER_SIZE, HOST, WEBHOST);
	// Initialize the Server
	server->init();
	// While the system has not eroded past its usefullness
	while (true)
	{
		// Handle Requests
		server->handleRequest();
	}
}