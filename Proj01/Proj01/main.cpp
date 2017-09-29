/*
 * ServerProcess.cpp
 * 
 * By Ryan Owens
 * 
 * CS447
 * 
 * Created September 8, 2017
 *
*/
#include "Proxy.h"

#define SERVER_PORT 9080
#define WEBHOST_PORT 1080
#define MAXCONNECTIONS 3
#define BUFFER_SIZE 1024
#define HOST "localhost"
#define WEBHOST "localhost"

int main()
{
	char hazardous_contents_CS_01[256] = "VIRUS";
	char hazardous_contents_CS_02[256] = "admin";

	char hazardous_contents_SC_01[256] = "";
	char hazardous_contents_SC_02[256] = "";

	// Get a new Server
	Proxy *server = new Proxy(SERVER_PORT, WEBHOST_PORT, MAXCONNECTIONS, BUFFER_SIZE, HOST, WEBHOST);
	// Initialize the Server
	server->init(hazardous_contents_CS_01, hazardous_contents_CS_02, hazardous_contents_SC_01, hazardous_contents_SC_02);
	// Handle Requests
	server->handleRequests();
}