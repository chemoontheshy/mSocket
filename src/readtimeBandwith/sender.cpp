/**
 *  ClassName:Sender
 *  Description：所有Activity的基类
 *  author: chemoontheshy
 *  date:2021-7-29
 */
#include "sender.h"

Sender::Sender()
{
	//初始化WSA
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0) return;
}

Sender::~Sender()
{
	sockFree();
}

void Sender::start(uint16_t Port ,const char *IP)
{
	auto sendSockfd = creatUdpSocket();
	if (!bindSocket(sendSockfd, "0.0.0.0", SENDER_UDP_PORT))
	{
		printf("failed to create serverRtpSockfd socket.");
		return;
	}
	//send
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(Port);
	inet_pton(AF_INET, IP, &sin.sin_addr);
	size_t num = 0;
	char buf[100]{ 0 };
	while (num < 120) {
		auto data_len = sendto(sendSockfd, buf,100, 0, reinterpret_cast<sockaddr*>(&sin), sizeof(sin));
		std::cout <<num<< " len: " << data_len << std::endl;
		Sleep(1000);
		num++;
	}
	sockFree();
}

void Sender::read(uint16_t Port)
{
	auto readSockfd = creatUdpSocket();
	if (!bindSocket(readSockfd, "0.0.0.0", Port))
	{
		printf("failed to create serverRtpSockfd socket.");
		return;
	}
	//接受rtp
	char buf[1024] = { 0 };
	sockaddr_in rtp_client;
	socklen_t rtp_client_len = sizeof(rtp_client);
	
	while (1) {
		//printf("waiting...\n");
		
		auto _size = recvfrom(readSockfd, buf, 1024, 0, reinterpret_cast<sockaddr*>(&rtp_client), &rtp_client_len);
		if (_size < 0) {
			//perror("recvfrom");
		}
		else if (_size == 0) {
			printf("client shutdown...\n");
		}
		else {
			auto  now_time = std::chrono::system_clock::now();
			auto duration_time = std::chrono::duration_cast<std::chrono::milliseconds>(now_time.time_since_epoch());
			std::cout << duration_time.count() << std::endl;
			printf(" get# %d\n", _size);
			memset(buf, 0, 1024);
		}
	}
}

SOCKET Sender::creatTcpSocket()
{
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET){
		printf("TCP::Socket() failed.\n");
		return INVALID_SOCKET;
	}
	return sock;
}

SOCKET Sender::creatUdpSocket()
{
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET) {
		printf("TCP::Socket() failed.\n");
		return INVALID_SOCKET;
	}
	return sock;
}

bool Sender::bindSocket(SOCKET sock, const char* ip, uint16_t port)
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

bool Sender::sockInit(SOCKET sock, const char* ip, uint16_t port, int ListenQueue)
{
	if (!Sender::bindSocket(sock, ip, port))
		return false;
	if (listen(sock, ListenQueue) == SOCKET_ERROR) {
		printf("TCP:listen() failed.\n");
		return false;
	}
	return true;
}

void Sender::sockFree()
{
	WSACleanup();
}
