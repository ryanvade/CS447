#include "ProxyServer.h"

#define SERVER_PORT 9080
#define WEBHOST_PORT 1080
#define MAXCONNECTIONS 3
#define BUFFER_SIZE 1024
#define HOST "localhost"
#define WEBHOST "localhost"
int main()
{
	// Get a new Server
	ProxyServer *server = new ProxyServer(SERVER_PORT, WEBHOST_PORT, MAXCONNECTIONS, BUFFER_SIZE, HOST, WEBHOST);
	// Initialize the Server
	server->init();
		// Handle Requests
		server->handleRequest();
}