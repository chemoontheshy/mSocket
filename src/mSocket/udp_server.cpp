#include "udp_server.h"

SUDP::SUDP()
{
	//≥ı ºªØWSA
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return;
	}

}

SUDP::~SUDP()
{
	WSACleanup();
}

void SUDP::start()
{
	auto udp_sock = Socket(AF_INET, SOCK_DGRAM);
	if (!Bind(udp_sock, "127.0.0.1", 8002)) {
		return;
	}
	return;
	auto send_sock = Socket(AF_INET, SOCK_DGRAM);
	char buf[1024];
	sockaddr_in client;
	socklen_t client_len = sizeof(client);
	int num = 0;
	while (1) {
		//printf("waiting...\n");
		size_t _size = recvfrom(udp_sock, buf, sizeof(buf) - 1, 0, reinterpret_cast<sockaddr*>(&client), &client_len);
		if (_size < 0) {
			perror("recvfrom");
		}
		else if (_size == 0) {
			printf("client shutdown...\n");
		}
		else {
			num++;
			printf("get# %d\n", num);
			
		}
	}
}


SOCKET SUDP::Socket(int af, int type, int protocol)
{
	SOCKET sock = socket(af, type, protocol);
	if (sock == INVALID_SOCKET) {
		printf("TCP::Socket() failed.\n");
		return INVALID_SOCKET;
	}
	return sock;
}

bool SUDP::Bind(SOCKET sock, const char* ip, uint16_t port)
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

bool SUDP::tcpSockInit(SOCKET sock, const char* ip, uint16_t port, size_t ListenQueue)
{
	if (!SUDP::Bind(sock, ip, port))
		return false;
	if (listen(sock, ListenQueue) == SOCKET_ERROR) {
		printf("TCP:listen() failed.\n");
		return false;
	}
	return true;
}

void SUDP::free()
{
	WSACleanup();
}