/**********************************************************************
 *  ClassName:sender.cpp
 *  Description：测试
 *  author: chemoontheshy
 *  date:2021-7-29
 *********************************************************************/
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
	char buf[1000]{ 0 };
	while (true) {
		auto data_len = sendto(sendSockfd, buf,100, 0, reinterpret_cast<sockaddr*>(&sin), sizeof(sin));
		//std::cout <<num<< " len: " << data_len << std::endl;
		//num++;
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
	//接受udp
	char buf[1500] = { 0 };
	sockaddr_in rtp_client;
	socklen_t rtp_client_len = sizeof(rtp_client);
	
	size_t count_num = 0;
	int64_t last_time = 0;
	int64_t mid_time = 0;
	int64_t now_time = 0;
	int64_t tol_time = 0;
	double tol_byte = 0.f;
	long double tol_bw = 0.f;
	size_t count_bt = 0;
	while (1) {
		//mid_time = utils::getUsec();
		auto _size = recvfrom(readSockfd, buf, 1500, 0, reinterpret_cast<sockaddr*>(&rtp_client), &rtp_client_len);
		now_time = utils::getUsec();
		if (_size < 0) {
			perror("recvfrom");
		}
		else if (_size == 0) {
			printf("client shutdown...\n");
		}
		else {
			if (last_time == 0) {
				last_time = utils::getUsec();
	
			}
			else {
				tol_byte = tol_byte + _size;
				count_num++;
				if (count_num==10) {
					//double bw = (tol_byte * 8 * 1000 * 1000) / ((now_time - last_time) * 1000 * 1000);
					double bw = (tol_byte * 8 ) / (now_time - last_time);
					tol_bw = tol_bw + bw;
					cout << "B:" << tol_byte <<" T: "<<now_time-last_time << endl;
					cout << "bw: " << bw << "Mbps" << endl;
					last_time = 0;
					count_num = 0;
					tol_byte = 0;
					count_bt++;
				}
			}

		}
	}
}

float Sender::delaySend(uint16_t Port, const char* IP)
{
	auto sendSockfd = creatUdpSocket();
	if (!bindSocket(sendSockfd, "0.0.0.0", SENDER_UDP_PORT))
	{
		printf("failed to create serverRtpSockfd socket.");
		return 1.0;
	}
	//send
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(Port);
	inet_pton(AF_INET, IP, &sin.sin_addr);
	size_t num = 0;
	char buf[1024]{ 0 };
	socklen_t sin_len = sizeof(sin);
	
	//time
	int64_t time_send = 0;
	auto data_len = sendto(sendSockfd, buf, 100, 0, reinterpret_cast<sockaddr*>(&sin), sizeof(sin));
	time_send = utils::getUsec();
	auto _size = recvfrom(sendSockfd, buf, 1024, 0, reinterpret_cast<sockaddr*>(&sin), &sin_len);
	return utils::getUsec() - time_send;
}

void Sender::delayReply(uint16_t Port)
{
	auto readSockfd = creatUdpSocket();
	if (!bindSocket(readSockfd, "0.0.0.0", Port))
	{
		printf("failed to create serverRtpSockfd socket.");
		return;
	}
	//接受udp
	char buf[1024] = { 0 };
	sockaddr_in rtp_client;
	socklen_t rtp_client_len = sizeof(rtp_client);

	//send
	auto sendSockfd = creatUdpSocket();
	if (!bindSocket(sendSockfd, "0.0.0.0", SENDER_UDP_PORT))
	{
		printf("failed to create serverRtpSockfd socket.");
	}
	//send
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(Port);
	inet_pton(AF_INET, "192.168.3.73", &sin.sin_addr);
	size_t num = 0;
	char buf[1024]{ 0 };
	socklen_t sin_len = sizeof(sin);
	while (1) {
		auto _size = recvfrom(readSockfd, buf, 1024, 0, reinterpret_cast<sockaddr*>(&rtp_client), &rtp_client_len);
		auto data_len = sendto(sendSockfd, _size, 100, 0, reinterpret_cast<sockaddr*>(&sin), sizeof(sin));
		
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
