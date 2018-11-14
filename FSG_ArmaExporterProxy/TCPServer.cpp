#include <iostream>
#include <string.h>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "DataProcess.hpp"

#define PORT 54000  // the port users will be connecting to

int main (void)
{
	listenAgain:
	//Create a socket
	int listening = socket (AF_INET, SOCK_STREAM, 0);
	if (listening == -1) {
		std::cerr << "Can't create a socket! Quitting" << std::endl;
		return 1;
	}

	//Bind IP and port to a socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons (PORT);
	hint.sin_addr.s_addr = INADDR_ANY; // Cloud also be INET_PTON

	bind (listening, (sockaddr*)&hint, sizeof (hint));

	//Tell winsock the socket is for listening
	listen (listening, SOMAXCONN);

	//Wait for connection
	sockaddr_in client;
	socklen_t clientSize = sizeof (client);

	int clientSocket = accept (listening, (sockaddr*)&client, &clientSize);

	char host[NI_MAXHOST]; //client's remote name
	char service[NI_MAXSERV]; //services (port) the client is connected on

	memset (host, 0, NI_MAXHOST);	//same as memset(host, 0, NI_MAXHOST);
	memset (service, 0, NI_MAXSERV);

	if (getnameinfo ((sockaddr*)&client, clientSize, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
		std::cout << host << " connected on port " << service << std::endl;
	}
	else {
		inet_ntop (AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << host << " connectd on port " << ntohs (client.sin_port) << std::endl;
	}

	//Close listening socket
	close (listening);

	//While loop: Accept and echo message back to client
	char buf[4096]; //limited message lenght, should be listening until end of character
	DataProcess* dataProcess;
	bool isInitialized = false;
	while (true) {
		memset (buf, 0, 4096);

		//Wait for client to send data
		long bytesReceived = recv (clientSocket, buf, 4096, 0);
		if (bytesReceived == -1) {
			std::cerr << "Error on recv(). Quitting" << std::endl;
			//delete dataProcess;
			break;
		}
		if (bytesReceived == 0) {
			std::cout << "Client disconnected" << std::endl;
			//delete dataProcess;
			break;
		}
		//Proccess Data Commands
		std::string data (buf);
		if (data.substr(0,6).compare("start-") == 0) {
			isInitialized = true;
			//Create File with mission Name & and DateTime
			dataProcess = new DataProcess(data.substr(6));
		}
		else if (data.substr (0, 5).compare ("start") == 0) {
			isInitialized = true;
			//Create File with mission DateTime
			dataProcess = new DataProcess();
		}
		else if ((data.substr (0, 5).compare ("end") == 0)) {
			//SEND DATA
			//TODO Send Data Over FTP
			//CLEAN DATA
			//delete dataProcess;
		}
		else if(isInitialized){
			dataProcess->appendData (data);
			//Echo message back to client
			send (clientSocket, buf, bytesReceived + 1, 0);
		}
		else {
			std::ostringstream ss;
			ss << "NotInitialized";
			send (clientSocket, ss.str ().c_str(), ss.str ().size() + 1, 0);
		}
		send (clientSocket, buf, bytesReceived + 1, 0);
	}

	//Close socket
	close (clientSocket);
	goto listenAgain;

	//Close app
	std::cout << "Closing..." << std::endl;
	sleep (3000);

	return 0;
}