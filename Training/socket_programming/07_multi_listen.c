#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include "header.h"
#include <signal.h>

int iSocket;

void CloseSocket(iSocket)
{
	close(iSocket);
	printf("Close\n");
	exit(EXIT_SUCCESS);
}

int MultiListener(char *pcIP, char *pcPort)
{
	struct sockaddr_in tServerAddr, tClientAddr;
	char acBuffer[BUFFERSIZE] = {0};
	char acReply[BUFFERSIZE] = "OK!";
	unsigned int uiServerAddrLen, uiClientAddrLen;
	int iReceived = 0;

	if ((iSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		printf("Failed to create socket\n");
	}

	struct ip_mreq group;
	group.imr_multiaddr.s_addr = inet_addr(pcIP);
	group.imr_interface.s_addr = htonl(INADDR_ANY);

	if ((setsockopt(iSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void*) &group, sizeof(group))) < 0)
	{
		printf("Fail to set socket\n");
		return FAIL;
	}

	memset(&tServerAddr, 0, sizeof(tServerAddr));
	tServerAddr.sin_family = AF_INET;
	tServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	tServerAddr.sin_port = htons(atoi(pcPort));
	uiServerAddrLen = sizeof(tServerAddr);

	// Bind the server socket
	if (bind(iSocket, (struct sockaddr *) &tServerAddr, uiServerAddrLen) < 0)
	{
		printf("[Server] Failed to bind the server socket\n");
		return FAIL;
	}

	while (1)
	{
		// Receive a message from the client
		uiClientAddrLen = sizeof(tClientAddr);
		if ((iReceived = recvfrom(iSocket, acBuffer, BUFFERSIZE, 0, (struct sockaddr *) &tClientAddr, &uiClientAddrLen)) < 0)
		{
			printf("[Server] Failed to receive initial bytes from client\n");
			return FAIL;
		}

		printf("[Server] Client connected: %s:%d\n", inet_ntoa(tClientAddr.sin_addr), ntohs(tClientAddr.sin_port));

		// Send the message back to client
		if (sendto(iSocket, acReply, iReceived, 0, (struct sockaddr *) &tClientAddr, uiClientAddrLen) < 0)
		{
			printf("[Server] Fail to send back\n");
			return FAIL;
		}

		acBuffer[iReceived] = '\0';
		printf("[Server] Receive message: %s\n", acBuffer);

	    memset(acBuffer, 0, BUFFERSIZE);

	}

	close(iSocket);
	return SUCCESS;
}


int main(int argc, char *argv[])
{
	if (argc == 3)
	{
        struct sigaction TSigHandler;
		TSigHandler.sa_handler = CloseSocket;
		sigemptyset(&TSigHandler.sa_mask);
		TSigHandler.sa_flags = 0;
		sigaction(SIGINT, &TSigHandler, NULL);   //signal control (ctrl + c)
		MultiListener(argv[1], argv[2]);
	}
	else
	{
		printf("Usage: %s <muliticast ip> <port>\n", argv[0]);
		return FAIL;
	}

	return SUCCESS;
}
