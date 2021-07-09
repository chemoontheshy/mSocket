#include "tcp_server.h"

STCP::STCP()
{
	//初始化WSA
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return;
	}

}

STCP::~STCP()
{
	WSACleanup();
}

void STCP::start()
{
	SOCKET serverSOCK = Socket(AF_INET, SOCK_STREAM);
	if (!tcpSockInit(serverSOCK, "0.0.0.0", SERVER_TCP_PORT)) {
		printf("failed to reate TCP socket.\n");
		exit(EXIT_FAILURE);
	}
	sockaddr_in cliAdrr;
	socklen_t addrLen = sizeof(cliAdrr);
	cliAdrr.sin_addr.S_un.S_addr = INADDR_ANY;
	char revData[254];
	while (true)
	{
		printf("等待连接...\n");
		auto cli_sockfd = accept(serverSOCK, reinterpret_cast<sockaddr*>(&cliAdrr), &addrLen);
		if (cli_sockfd == INVALID_SOCKET) {
			printf("accpet error()");
			return;
		}
		char IPv4[16]{ 0 };
		fprintf(stdout, "Connection from %s:%d\n", inet_ntop(AF_INET, &cliAdrr.sin_addr, IPv4, sizeof(IPv4)),
			ntohs(cliAdrr.sin_port));
		int ret = recv(cli_sockfd, revData, 254, 0);
		if (ret > 0)
		{
			revData[ret-1] = 0x00;
			printf(revData);
			printf("\n");
		}

		//发送数据
		const char* sendData = "你好，TCP客户端！\n";
		send(cli_sockfd, revData, strlen(revData), 0);
		closesocket(cli_sockfd);
	}
	closesocket(serverSOCK);

}


SOCKET STCP::Socket(int af, int type, int protocol)
{
	SOCKET sock = socket(af, type, protocol);
	if (sock == INVALID_SOCKET) {
		printf("TCP::Socket() failed.\n");
		return INVALID_SOCKET;
	}
	return sock;
}

bool STCP::Bind(SOCKET sock, const char* ip, uint16_t port)
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

bool STCP::tcpSockInit(SOCKET sock, const char* ip, uint16_t port, size_t ListenQueue)
{
	if (!STCP::Bind(sock, ip, port))
		return false;
	if (listen(sock, ListenQueue) == SOCKET_ERROR) {
		printf("TCP:listen() failed.\n");
		return false;
	}
	return true;
}

void STCP::free()
{

}