#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include "header.h"
#include <signal.h>

int MultiSender(char *pcIP, char *pcPort, char *pcMsg)
{
	int iClientSocket;
	unsigned int uiSendAddrLen, uiRecvAddrLen;
	struct sockaddr_in tSendAddr, tRecvAddr;
	char acBuffer[BUFFERSIZE] = {0};
	int iMsgLen, iReceived;
	int iMulticast = 1;

	if ((iClientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		printf("[Client] Failed to create socket\n");
		return FAIL;
	}

	if (setsockopt(iClientSocket, IPPROTO_IP, IP_MULTICAST_TTL, &iMulticast, sizeof(iMulticast)) < 0)
	{
		printf("[Server] Fail to set broadcast\n");
		return FAIL;
	}

	memset(&tSendAddr, 0, sizeof(tSendAddr));
	tSendAddr.sin_family = AF_INET;
	tSendAddr.sin_addr.s_addr = inet_addr(pcIP);
	tSendAddr.sin_port = htons(atoi(pcPort));
	uiSendAddrLen = sizeof(tSendAddr);

	// Send the word to the server
	iMsgLen = strlen(pcMsg);
	if (sendto(iClientSocket, pcMsg, iMsgLen, 0, (struct sockaddr *) &tSendAddr, uiSendAddrLen) < 0)
	{
		printf("[Client] Could not send data\n");
		return FAIL;
	}

	printf("[Client] Send to %s:%d\n", inet_ntoa(tSendAddr.sin_addr), ntohs(tSendAddr.sin_port));

	// Receive the word back from the server
	uiRecvAddrLen = sizeof(tRecvAddr);
	if ((iReceived = recvfrom(iClientSocket, acBuffer, BUFFERSIZE, 0, (struct sockaddr *) &tRecvAddr, &uiRecvAddrLen)) < 0)
	{
		printf("[Client] Fail to receive from server\n");
		return FAIL;
	}

	acBuffer[iReceived] = '\0';
	printf("[Client] Server reply: %s\n", acBuffer);

	close(iClientSocket);

	return SUCCESS;

}

int main(int argc, char *argv[])
{
	if (argc == 4)
	{
		// Server function
		MultiSender(argv[1], argv[2], argv[3]);
	}
	else
	{
		printf("Usage: %s <muliticast ip> <port> <message>\n", argv[0]);
		return FAIL;
	}

	return SUCCESS;
}
