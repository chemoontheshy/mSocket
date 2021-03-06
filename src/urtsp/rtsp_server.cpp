#include "rtsp_server.h"
#include <stdio.h>
#include <stdlib.h>

RTSP::RTSP()
{
	//??ʼ??WSA
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return;
	}

}

RTSP::~RTSP()
{
	WSACleanup();
}

void RTSP::start(uint16_t clientRtpPort)
{
	
}



SOCKET RTSP::creatTcpSocket()
{
	SOCKET sock = socket(AF_INET,SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		printf("TCP::Socket() failed.\n");
		return INVALID_SOCKET;
	}
	return sock;
}

SOCKET RTSP::creatUdpSocket()
{
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET) {
		printf("TCP::Socket() failed.\n");
		return INVALID_SOCKET;
	}
	return sock;
}

bool RTSP::bindSocket(SOCKET sock, const char* ip, uint16_t port)
{
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
		printf("TCP::bind() failed.\n");
		return false;
	}
	return true;

}


bool RTSP::sockInit(SOCKET sock, const char* ip, uint16_t port, int ListenQueue)
{
	if (!RTSP::bindSocket(sock, ip, port))
		return false;
	if (listen(sock, ListenQueue) == SOCKET_ERROR) {
		printf("TCP:listen() failed.\n");
		return false;
	}
	return true;
}
void RTSP::free()
{
	WSACleanup();
}

char* RTSP::lineParser(char* src, char* line)
{
	while (*src != '\n')
		*(line++) = *(src++);

	*line = '\n';
	*(++line) = 0;
	return (src + 1);
}