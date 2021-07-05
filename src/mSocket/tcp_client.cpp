#include "tcp_client.h"

CTCP::CTCP()
{
	//初始化WSA
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return;
	}

}

CTCP::~CTCP()
{
	free();
}

void CTCP::start()
{
	auto sclient = Socket(AF_INET, SOCK_STREAM);
	if (!Connect(sclient, "127.0.0.1", 6000)) {
		printf("connect error !");
		closesocket(sclient);
		return;
	}
	const char* sendData = "你好，TCP服务端，我是客户端!\n";
	send(sclient, sendData, strlen(sendData), 0);

	char recData[255];
	int ret = recv(sclient, recData, 255, 0);
	if (ret > 0)
	{
		recData[ret] = 0x00;
		printf(recData);
	}
	closesocket(sclient);
	free();
}


SOCKET CTCP::Socket(int af, int type, int protocol)
{
	SOCKET sock = socket(af, type, protocol);
	if (sock == INVALID_SOCKET) {
		printf("TCP::Socket() failed.\n");
		return INVALID_SOCKET;
	}
	return sock;
}

bool CTCP::Connect(SOCKET sock, const char* ip, uint16_t port)
{
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	//addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	inet_pton(AF_INET, ip, &addr.sin_addr.S_un.S_addr);
	if (connect(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
		printf("TCP::connect() failed.\n");
		return false;
	}
	return true;

}


void CTCP::free()
{
	WSACleanup();
}