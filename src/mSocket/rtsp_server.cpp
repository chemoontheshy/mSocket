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

void RTSP::start()
{
	auto servRtspSockfd = Socket(AF_INET, SOCK_STREAM);
	if (!RTSP::rtspSockInit(servRtspSockfd, "0.0.0.0", SERVER_RTSP_PORT)) {
		fprintf(stderr, "failed to create RTSP socket. \n");
		exit(EXIT_FAILURE);
	}
	fprintf(stdout, "rtsp://127.0.0.1:%d\n", SERVER_RTSP_PORT);
	sockaddr_in cliAdrr;
	socklen_t addrLen = sizeof(cliAdrr);
	cliAdrr.sin_addr.S_un.S_addr = INADDR_ANY;
	printf("等待连接...\n");
	auto cli_sockfd = accept(servRtspSockfd, reinterpret_cast<sockaddr*>(&cliAdrr), &addrLen);
	if (cli_sockfd == INVALID_SOCKET) {
		printf("accpet error()");
		return;
	}
	char IPv4[16]{ 0 };
	fprintf(stdout, "Connection from %s:%d\n", inet_ntop(AF_INET, &cliAdrr.sin_addr, IPv4, sizeof(IPv4)),
		ntohs(cliAdrr.sin_port));
	char method[10]{ 0 };
	char url[100]{ 0 };
	char version[10]{ 0 };
	char line[500]{ 0 };
	int cseq;
	char recvBuf[1024]{ 0 };
	char sendBuf[1024]{ 0 };
	while (true)
	{
		int ret = recv(cli_sockfd, recvBuf, 254, 0);
		if (ret > 0)
		{
			fprintf(stdout, "--------------- [C->S] --------------\n");
			fprintf(stdout, "%s\n", recvBuf);
			char* bufferPtr = RTSP::lineParser(recvBuf, line);
			/* 解析method,url,version */
			if (sscanf_s(line, "%s %s %s\r\n", method, 10, url,100, version,10) != 3)
			{
				fprintf(stdout, "RTSP::lineParser() parse method error\n");
				break;
			}
			//std::cout <<"method: "<< method << std::endl;
			//std::cout << "url: " << url << std::endl;
			//std::cout << "version: " << version << std::endl;
			/* 解析序列号 */
			bufferPtr = RTSP::lineParser(bufferPtr, line);
			if (sscanf_s(line, "CSeq: %d\r\n", &cseq) != 1)
			{
				fprintf(stdout, "RTSP::lineParser() parse seq error\n");
				break;
			}
			std::cout << "method: " << method << std::endl;
			if (!strcmp(method, "SETUP"))
			{
				while (true)
				{
					bufferPtr = RTSP::lineParser(bufferPtr, line);
					if (!strncmp(line, "Transport:", sizeof("Transport:")))
					{
						sscanf_s(line, "Transport: RTP/AVP;unicast;client_port=%d-%d\r\n", &this->cliRtpPort, &this->cliRtcpPort);
					}
				}
			}
			if (!strcmp(method, "OPTIONS"))
				RTSP::replyCmd_OPTIONS(sendBuf, sizeof(sendBuf), cseq);
			else if (!strcmp(method, "DESCRIBE"))
				RTSP::replyCmd_DESCRBE(sendBuf, sizeof(sendBuf), cseq, url);
			else if (!strcmp(method, "SETUP"))
				std::cout << "test" << std::endl;
				//RTSP::replayCmd_SETUP(sendBuf, sizeof(sendBuf),cseq,this->cliRtpPort,)
			fprintf(stdout, "--------------- [S->C] --------------\n");
			fprintf(stdout, "%s\n", sendBuf);
			if (send(cli_sockfd, sendBuf, sizeof(sendBuf), 0) < 0) {
				printf("RTSP:serverClient() sned() failed\n");
				break;
			}


		}
		
	}
	closesocket(cli_sockfd);
	closesocket(servRtspSockfd);
	
}


SOCKET RTSP::Socket(int af, int type, int protocol)
{
	SOCKET sock = socket(af, type, protocol);
	if (sock == INVALID_SOCKET) {
		printf("TCP::Socket() failed.\n");
		return INVALID_SOCKET;
	}
	return sock;
}

bool RTSP::Bind(SOCKET sock, const char* ip, uint16_t port)
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

bool RTSP::rtspSockInit(SOCKET sock, const char* ip, uint16_t port, int ListenQueue)
{
	if (!RTSP::Bind(sock, ip, port))
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