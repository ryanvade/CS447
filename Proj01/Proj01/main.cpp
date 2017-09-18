#include "Proxy.h"

#define SERVER_PORT 9080
#define WEBHOST_PORT 1080
#define MAXCONNECTIONS 3
#define BUFFER_SIZE 1024
#define HOST "localhost"
#define WEBHOST "localhost"

const char* HAZARD = "hazard";

int main()
{
	// Get a new Server
	Proxy *server = new Proxy(SERVER_PORT, WEBHOST_PORT, MAXCONNECTIONS, BUFFER_SIZE, HOST, WEBHOST, HAZARD);
	// Initialize the Server
	server->init();
	// Handle Requests
	server->handleRequest();
}