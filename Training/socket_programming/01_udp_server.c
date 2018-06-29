#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "header.h"
#include <signal.h>

int iServerSocket;

void CloseSocket(iServerSocket)   //Ctrl+C 要關掉socket
{
	close(iServerSocket);
	printf("Close\n");
	exit(EXIT_SUCCESS);
}

int Udp_server(char *pcServerPort)
{
	unsigned int uiServerAddrLen, uiClientAddrLen;
	struct sockaddr_in tServerAddr, tClientAddr;
	char acBuffer[BUFFERSIZE] = {0};
	char acReply[BUFFERSIZE] = "OK!";
	int iReceived = 0;

	// Create the UDP socket
	if ((iServerSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		printf("[Server] Failed to create socket\n");
		return FAIL;
	}

	// Naming & address socket
	memset(&tServerAddr, 0, sizeof(tServerAddr));
	tServerAddr.sin_family = AF_INET;
	tServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	tServerAddr.sin_port = htons(atoi(pcServerPort));
	uiServerAddrLen = sizeof(tServerAddr);

	// Bind the server socket
	if (bind(iServerSocket, (struct sockaddr *) &tServerAddr, uiServerAddrLen) < 0)
	{
		printf("[Server] Failed to bind the server socket\n");
		close(iServerSocket);
		return FAIL;
	}

	/*struct sigaction TSigHandler;               //Signal control (Ctrl + C)
	TSigHandler.sa_handler = CloseSocket;
	sigemptyset(&TSigHandler.sa_mask);
	TSigHandler.sa_flags = 0;
	sigaction(SIGINT, &TSigHandler, NULL);   //signal control (ctrl + c)
*/
	// Run until cancelled
	while (1)
	{
		// Receive a message from the client
		uiClientAddrLen = sizeof(tClientAddr);
		if ((iReceived = recvfrom(iServerSocket, acBuffer, BUFFERSIZE, 0, (struct sockaddr *) &tClientAddr, &uiClientAddrLen)) < 0)
		{
			printf("[Server] Failed to receive initial bytes from client\n");
			close(iServerSocket);
			return FAIL;
		}

		printf("[Server] Client connected: %s:%d\n", inet_ntoa(tClientAddr.sin_addr), ntohs(tClientAddr.sin_port));

		// Send the message back to client
		if (sendto(iServerSocket, acReply, iReceived, 0, (struct sockaddr *) &tClientAddr, uiClientAddrLen) < 0)
		{
			printf("[Server] Fail to send back\n");
			close(iServerSocket);
			return FAIL;
		}

		acBuffer[iReceived] = '\0';
		printf("[Server] Receive message: %s\n", acBuffer);

	    memset(acBuffer, 0, BUFFERSIZE); //Reset buffer

	//	sigaction(SIGINT, &TSigHandler, NULL);   //signal control (ctrl + c)
	}

	close(iServerSocket);

	return SUCCESS;
}


int main(int argc, char *argv[])
{
	if (argc == 2)
	{
		// Server functioni
        struct sigaction TSigHandler;
		TSigHandler.sa_handler = CloseSocket;
		sigemptyset(&TSigHandler.sa_mask);
		TSigHandler.sa_flags = 0;
		sigaction(SIGINT, &TSigHandler, NULL);   //signal control (ctrl + c)
		Udp_server(argv[1]);
	}
	else
	{
		printf("Usage: %s <port>\n", argv[0]);
		return FAIL;
	}

	return SUCCESS;
}
