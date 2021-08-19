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
	size_t send_byte = 1 * 1000 * 1000;
	size_t num = 0;
	char buf[1000]{ 0 };
	while (true) {
		send_byte = send_byte - 1000;
		if (send_byte == 0) {
			buf[0] = 1;
			send_byte == 1 * 1000 * 1000;
		}
		auto data_len = sendto(sendSockfd, buf,1000, 0, reinterpret_cast<sockaddr*>(&sin), sizeof(sin));
		//std::cout <<num<< " len: " << data_len << std::endl;
		buf[0] = 0;
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
		if (last_time == 0) {
			last_time = utils::getUsec();
			continue;
		}
		now_time = utils::getUsec();
		if (_size < 0) {
			perror("recvfrom");
		}
		else if (_size == 0) {
			printf("client shutdown...\n");
		}
		else {
			tol_byte = tol_byte + _size;
			count_num++;
			//cout << utils::getUsec()<< endl;
			//std::cout << "bw: " << (_size * 8) / static_cast <float>(now_time - last_time) << "Mbps" << std::endl;
			if (count_num == 10) {
				double bw = (tol_byte * 8) / (now_time - last_time);
				cout << "B:" << tol_byte << " T: " << now_time - last_time << endl;
				cout << "bw: " << bw << "Mbps" << endl;
				last_time = 0;
				count_num = 0;
				tol_byte = 0;
			}

		}
	}
}

float Sender::delaySend(uint16_t Port, const char* IP)
{

	auto readSockfd = creatUdpSocket();
	if (!bindSocket(readSockfd, "0.0.0.0", Port))
	{
		printf("failed to create serverRtpSockfd socket.");
		return 0.f;
	}
	//接受udp
	char buf[1024] = { 0 };
	sockaddr_in rtp_client;
	socklen_t rtp_client_len = sizeof(rtp_client);
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
	socklen_t sin_len = sizeof(sin);
	
	//time
	int64_t time_f = 0;
	int64_t time_b = 0;
	while (1) {
		//time_f = utils::getUsec();
		auto data_len = sendto(sendSockfd, buf, 8, 0, reinterpret_cast<sockaddr*>(&sin), sizeof(sin));
		time_b = utils::getUsec();
		auto _size = recvfrom(readSockfd, buf, 8, 0, reinterpret_cast<sockaddr*>(&rtp_client), &rtp_client_len);
		float rtt = static_cast<float>(utils::getUsec() - time_b) / 1000;
		//cout << "抖动:" << static_cast<float>(time_b - time_f) / 1000 << "ms" << endl;
		cout << "rtt:" << rtt << "ms" << endl;
	}
	
	return 0.f;
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
	socklen_t sin_len = sizeof(sin);
	while (1) {
		auto _size = recvfrom(readSockfd, buf, 1024, 0, reinterpret_cast<sockaddr*>(&rtp_client), &rtp_client_len);
		auto data_len = sendto(sendSockfd, buf, _size, 0, reinterpret_cast<sockaddr*>(&sin), sizeof(sin));
		
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
