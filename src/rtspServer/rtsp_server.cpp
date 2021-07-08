#include "rtsp_server.h"
#include <stdio.h>
#include <stdlib.h>

RTSP::RTSP()
{
	//初始化WSA
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
	auto serverSockfd = creatTcpSocket();
	if (!RTSP::sockInit(serverSockfd, "0.0.0.0", SERVER_PORT))
	{
		printf("failed to create server socket.");
		return;
	}
	//接收码流
	auto clientRtpSockfd = creatUdpSocket();
	if (!bindSocket(clientRtpSockfd, "0.0.0.0", clientRtpPort))
	{
		printf("failed to create clientRtp socket.");
		return;
	}
	//rtpSocket
	auto serverRtpSockfd = creatUdpSocket();
	if (!bindSocket(serverRtpSockfd, "0.0.0.0", SERVER_RTP_PORT))
	{
		printf("failed to create serverRtpSockfd socket.");
		return;
	}
	//rtcpSocket
	auto serverRtcpSockfd = creatUdpSocket();
	if (!bindSocket(serverRtcpSockfd, "0.0.0.0", SERVER_RTP_PORT))
	{
		printf("failed to create serverRtcpSockfd socket.");
		return;
	}
	printf("rtsp:://127.0.0.1:%d\n", SERVER_PORT);

	printf("end");
}



SOCKET RTSP::creatTcpSocket()
{
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
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
	inet_pton(AF_INET, "127.0.0.1", &sockAddr.sin_addr.s_addr);
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