#include "ProxyServer.h"

#define SERVER_PORT 9080
#define WEBHOST_PORT 80
#define MAXCONNECTIONS 3
#define BUFFER_SIZE 100
#define HOST "localhost"
#define WEBHOST "174.138.61.250"
int main()
{
	// Get a new Server
	ProxyServer *server = new ProxyServer(SERVER_PORT, WEBHOST_PORT, MAXCONNECTIONS, BUFFER_SIZE, HOST, WEBHOST);
	// Initialize the Server
	server->init();
	// While the system has not eroded past its usefullness
	while (true)
	{
		// Handle Requests
		server->handleRequest();
	}
}