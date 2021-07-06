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
	servRtspSockfd = Socket(AF_INET, SOCK_STREAM);
	if (!RTSP::rtspSockInit(servRtspSockfd, "0.0.0.0", SERVER_RTSP_PORT)) {
		fprintf(stderr, "failed to create RTSP socket. \n");
		exit(EXIT_FAILURE);
	}
	servRtpSockfd = RTSP::Socket(AF_INET, SOCK_DGRAM);
	if (!RTSP::Bind(servRtpSockfd, "0.0.0.0", SERVER_RTP_PORT))
	{
		fprintf(stderr, "failed to create RTP socket. \n");
		exit(EXIT_FAILURE);
	}
	
	fprintf(stdout, "rtsp://127.0.0.1:%d\n", SERVER_RTSP_PORT);
	sockaddr_in cliAddr;
	socklen_t addrLen = sizeof(cliAddr);
	cliAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	printf("等待连接...\n");
	auto cli_sockfd = accept(servRtspSockfd, reinterpret_cast<sockaddr*>(&cliAddr), &addrLen);
	if (cli_sockfd == INVALID_SOCKET) {
		printf("accpet error()");
		return;
	}
	char IPv4[16]{ 0 };
	fprintf(stdout, "Connection from %s:%d\n", inet_ntop(AF_INET, &cliAddr.sin_addr, IPv4, sizeof(IPv4)),
		ntohs(cliAddr.sin_port));
	int ssrcNum = 19990101;
	const char* sessionID = "ivisionic";
	int timeout = 60;
	float fps = 60;
	this->serveClient(cli_sockfd, cliAddr, servRtpSockfd, ssrcNum, sessionID, timeout, fps);

	//再次建立
	auto second_sockfd = accept(servRtspSockfd, reinterpret_cast<sockaddr*>(&cliAddr), &addrLen);
	if (second_sockfd == INVALID_SOCKET) {
		printf("accpet error()");
		return;
	}
	this->serveClient(second_sockfd, cliAddr, servRtpSockfd, ssrcNum, sessionID, timeout, fps);
	
}
void RTSP::serveClient(SOCKET clientfd, const sockaddr_in& cliAddr, SOCKET rtpFD, const int ssrcNum, const char* sessionID, const int timeout, const float fps)
{
	char method[10]{ 0 };
	char url[100]{ 0 };
	char version[10]{ 0 };
	char line[500]{ 0 };
	int cseq;
	size_t heartbeatCount = 0;
	char recvBuf[1024]{ 0 };
	char sendBuf[1024]{ 0 };
	while (true)
	{
		auto recvLen = recv(clientfd, recvBuf, sizeof(sendBuf) - 1, 0);
		if (recvLen <= 0)
			break;
		recvBuf[recvLen] = 0;
		fprintf(stdout, "--------------- [C->S] --------------\n");
		fprintf(stdout, "%s", recvBuf);
		char* bufferPtr = RTSP::lineParser(recvBuf, line);
		/* 解析method,url,version */
		if (sscanf_s(line, "%s %s %s\r\n", method, 10, url, 100, version, 10) != 3)
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
				if (!strncmp(line, "Transport:", strlen("Transport:")))
				{
					sscanf_s(line, "Transport: RTP/AVP;unicast;client_port=%d-%d\r\n", &this->cliRtpPort, &this->cliRtcpPort);
					break;
				}
			}
		}
		if (!strcmp(method, "OPTIONS"))
			RTSP::replyCmd_OPTIONS(sendBuf, sizeof(sendBuf), cseq);
		else if (!strcmp(method, "DESCRIBE"))
			RTSP::replyCmd_DESCRBE(sendBuf, sizeof(sendBuf), cseq, url);
		else if (!strcmp(method, "SETUP"))
			RTSP::replayCmd_SETUP(sendBuf, sizeof(sendBuf), cseq, this->cliRtpPort, 19950715, "ivisionic", 60);
		else if (!strcmp(method, "PLAY"))
			RTSP::replyCmd_PLAY(sendBuf, sizeof(sendBuf)-1, cseq, "ivisionic", 60);
		else
		{
			fprintf(stderr, "Parse method error\n");
			break;
		}
		fprintf(stdout, "--------------- [S->C] --------------\n");
		fprintf(stdout, "%s\n", sendBuf);
		if (send(clientfd, sendBuf, sizeof(sendBuf), 0) < 0) {
			printf("RTSP:serverClient() sned() failed\n");
			break;
		}
		if (!strcmp(method, "PLAY"))
		{
			char IPv4[16]{ 0 };
			inet_ntop(AF_INET, &cliAddr.sin_addr, IPv4, sizeof(IPv4));
			char buf[1024] = {0};
			sockaddr_in client;
			socklen_t client_len = sizeof(client);
			int num = 0;
			//send
			fprintf(stderr, "this->cliRtpPort:%d\n",this->cliRtpPort);
			auto udp_sockfd = RTSP::Socket(AF_INET, SOCK_DGRAM);
			sockaddr_in udp_client;
			udp_client.sin_family = AF_INET;
			inet_pton(udp_client.sin_family, IPv4, &udp_client.sin_addr);
			udp_client.sin_port = htons(this->cliRtpPort);
			int udp_client_len = sizeof(udp_client);
			while (1) {
				
				//printf("waiting...\n");
				size_t _size = recvfrom(rtpFD, buf, sizeof(buf), 0, reinterpret_cast<sockaddr*>(&client), &client_len);
				if (_size < 0) {
					perror("recvfrom");
				}
				else if (_size == 0) {
					printf("client shutdown...\n");
				}
				else {
					//num++;
					printf("get# %lld\n", _size);
					//printf(b);
					auto sentBytes =  sendto(udp_sockfd,buf, sizeof(buf),0, reinterpret_cast<sockaddr*>(&udp_client), udp_client_len);
					std::cout << "sentBytes:" << sentBytes << std::endl;
					memset(buf, 0, 1024);
				}
			}
		}
	}
	closesocket(clientfd);
	
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