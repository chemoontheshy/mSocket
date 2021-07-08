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
	auto udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(4002);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(udp_sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
		printf("TCP::bind() failed.\n");
		return;
	}
	int num = 0;
	char buf[1024]={ 0 };
	sockaddr_in client;
	socklen_t client_len = sizeof(client);
	//send 
	auto send_sock = socket(AF_INET, SOCK_DGRAM, 0);
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(8888);
	sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int len = sizeof(sin);
	char* rBuf = (char*)malloc(1024*1024);
	while (1) {
		//printf("waiting...\n");
		auto _size = recvfrom(udp_sock, rBuf, 1024, 0, reinterpret_cast<sockaddr*>(&client), &client_len);
		if (_size < 0) {
			perror("recvfrom");
		}
		else if (_size == 0) {
			printf("client shutdown...\n");
		}
		else {
			printf("get# %d\n", _size);
			rBuf[_size] = '\0';
			auto cout= sendto(send_sock, rBuf, _size, 0, (sockaddr*)&sin, len);
			printf("get# %d\n", cout );
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