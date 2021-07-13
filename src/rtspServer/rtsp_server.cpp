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
	if (!bindSocket(serverRtcpSockfd, "0.0.0.0", SERVER_RTCP_PORT))
	{
		printf("failed to create serverRtcpSockfd socket.");
		return;
	}
	printf("rtsp://127.0.0.1:%d\n", SERVER_PORT);
	while (true)
	{
		//接收初始
		sockaddr_in cliAddr;
		socklen_t len = sizeof(cliAddr);
		memset(&cliAddr, 0, len);
		auto clientSockfd = accept(serverSockfd, reinterpret_cast<sockaddr*>(&cliAddr),&len);
		if (clientSockfd == INVALID_SOCKET) {
			printf("failed to accept client\n");
			break;
		}
		char IPv4[16]{ 0 };
		//printf("accept client;client ip:%ld,client port:%ld\n", cliAddr.sin_addr.S_un.S_addr, cliAddr.sin_port);
		auto ip_S_adrr = cliAddr.sin_addr.S_un.S_addr;
		printf("Connection from %s:%d\n", inet_ntop(AF_INET, &cliAddr.sin_addr, IPv4, sizeof(IPv4)),ntohs(cliAddr.sin_port));
		auto endTCP = serveClient(clientSockfd, clientRtpSockfd, serverRtpSockfd, serverRtcpSockfd, ip_S_adrr);
		if (!endTCP)
			break;
	}

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

bool RTSP::serveClient(SOCKET serverSockfd, SOCKET clientRtpSockfd, SOCKET serverRtpSockfd,
	SOCKET serverRtcpSockfd,ULONG ip_S_adrr)
{
	printf("test\n");
	char method[10]{0};
	char url[100]{ 0 };
	char version[10]{ 0 };
	int cseq;
	int serverRtpPort = -1;
	int serverRtcpPort = -1;
	char* bufPtr;
	char* rBuf = (char*)malloc(MAX_SIZE);
	char* sBuf = (char*)malloc(MAX_SIZE);
	if (rBuf == nullptr || sBuf == nullptr)
		return false;
	char line[400];
	while (true)
	{
		int recvLen = -1;
		if (rBuf == nullptr)
			break;
		recvLen = recv(serverSockfd, rBuf, MAX_SIZE-1, 0);
		if (recvLen < 0)
			break;
		rBuf[recvLen]='\0';
		printf("---------------C->S--------------\n");
		printf("%s", rBuf);

		/* 解析方法*/
		bufPtr = lineParser(rBuf, line);
		if (sscanf_s(line, "%s %s %s\r\n", method, 10, url, 100, version, 10)!=3)
		{
			fprintf(stdout, "RTSP::lineParser() parse method error\n");
			break;
		}

		/* 解析序列号 */
		/*bufPtr = lineParser(bufPtr, line);
		if (sscanf_s(line, "CSeq: %d\r\n", &cseq) != 1)
		{
			std::cout << sscanf_s(line, "CSeq: %d\r\n", &cseq) << std::endl;
			fprintf(stdout, "RTSP::lineParser() parse seq error\n");
			break;
		}*/
		while (true)
		{
			bufPtr = lineParser(bufPtr, line);
			if (!strncmp(line, "CSeq:", strlen("CSeq:")))
			{
				sscanf_s(line, "CSeq: %d\r\n", &cseq);
				break;
			}
		}
		if (!strcmp(method, "SETUP"))
		{
			while (true)
			{
				bufPtr = lineParser(bufPtr, line);
				if (!strncmp(line, "Transport:", strlen("Transport:")))
				{
					sscanf_s(line, "Transport: RTP/AVP;unicast;client_port=%d-%d\r\n", &serverRtpPort,&serverRtcpPort);
					break;
				}
			}
		}
		if (!strcmp(method, "OPTIONS"))
			RTSP::replyCmd_OPTIONS(sBuf, MAX_SIZE, cseq);
		else if (!strcmp(method, "DESCRIBE"))
			RTSP::replyCmd_DESCRBE(sBuf, MAX_SIZE, cseq, url);
		else if (!strcmp(method, "SETUP"))
			RTSP::replayCmd_SETUP(sBuf, MAX_SIZE, cseq, serverRtpPort, 19950715, "ivisionic", 60);
		else if (!strcmp(method, "PLAY"))
			RTSP::replyCmd_PLAY(sBuf, MAX_SIZE, cseq, "ivisionic", 60);
		else if (!strcmp(method, "TEARDOWN"))
			RTSP::replyCmd_TEARDOWN(sBuf, MAX_SIZE, cseq);
		else
		{
			fprintf(stderr, "Parse method error\n");
			break;
		}
		printf("---------------S->C--------------\n");
		printf("%s", sBuf);
		if (sBuf == nullptr)
			break;
		if (send(serverSockfd, sBuf, static_cast<int>(strlen(sBuf)), 0) < 0) {
			printf("RTSP:serverClient() sned() failed\n");
			break;
		}
		if (!strcmp(method, "PLAY"))
		{
			std::thread t1(thread_do,clientRtpSockfd,serverRtpSockfd,ip_S_adrr,serverRtpPort);
			t1.detach();
		}
		if (!strcmp(method, "TEARDOWN"))
			break;
	}
	closesocket(serverSockfd);
	closesocket(clientRtpSockfd);
	closesocket(serverRtcpSockfd);
	closesocket(serverRtcpSockfd);
	return false;
}

void RTSP::sockFree()
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

void RTSP::thread_do(SOCKET clientRtpSockfd, SOCKET serverRtpSockfd,ULONG ip_S_adrr,const int serverRtpPort)
{
	int num = 0;
	int rtcp_num = 0;
	char buf[1024] = { 0 };
	char mbuf[1024] = { 0 };
	//接受rtp
	sockaddr_in rtp_client;
	socklen_t rtp_client_len = sizeof(rtp_client);
	//send
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(serverRtpPort);
	//sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	//新函数
	//inet_pton(AF_INET, "127.0.0.1", &sin.sin_addr);
	sin.sin_addr.S_un.S_addr = ip_S_adrr;
	int len = sizeof(sin);
	while (1) {
		//printf("waiting...\n");
		auto _size = recvfrom(clientRtpSockfd, buf, 1024, 0, reinterpret_cast<sockaddr*>(&rtp_client), &rtp_client_len);
		if (_size < 0) {
			//perror("recvfrom");
		}
		else if (_size == 0) {
			printf("client shutdown...\n");
		}
		else {
			//num++;
			auto cout = sendto(serverRtpSockfd, buf, _size, 0, reinterpret_cast<sockaddr*>(&sin), len);
			//printf("get# %d\n", cout);
			memset(buf, 0, 1024);
		}
	}
}