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
	size_t m_num = 100;
	int m_length = 100;
	char buf[1000]{ 0 };
	while (true) {
		if (num > m_num)
		{
			buf[0] = 1;
			auto data_len = sendto(sendSockfd, buf, m_length, 0, reinterpret_cast<sockaddr*>(&sin), sizeof(sin));
			//std::cout <<num<< " len: " << data_len << std::endl;
			buf[0] = 0;
			num = 0;
			Sleep(1000);
			continue;
		}
		auto data_len = sendto(sendSockfd, buf, m_length, 0, reinterpret_cast<sockaddr*>(&sin), sizeof(sin));
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
	while (1) {
		auto _size = recvfrom(readSockfd, buf, 1500, 0, reinterpret_cast<sockaddr*>(&rtp_client), &rtp_client_len);
		if (last_time == 0) {
			last_time = utils::getUsec();
			continue;
		}
		if (_size < 0) {
			perror("recvfrom");
		}
		else if (_size == 0) {
			printf("client shutdown...\n");
		}
		else {
			tol_byte = tol_byte + _size;
			//cout << "et"<< endl;
			if (buf[0] == '1') {
				if (tol_byte != 100000) {
					//cout << "tol_byte:"<<tol_byte << endl;
					tol_byte = 0;
					last_time = 0;
					continue;
				}
				count_num++;
				now_time = utils::getUsec();
				double bw = (tol_byte * 8*1024*1024) / (now_time - last_time)/1000/1000;
				//cout << "now_time:" << now_time << " last_time: " << last_time << endl;
				
				//cout << "B:" << tol_byte << " T: " << now_time - last_time << endl;
				//cout << "bw: " << bw << "Mbps" << endl;
				//data_100_10_10,数据-字节长度-1组packet数-瓶颈带宽。
				//data_1kbps_5x25_10,数据-字节长度*1组packet数-瓶颈带宽。
				fstream f;
				f.open("data_100_1000_100.txt", ios::out | ios::app);
				f << count_num << " B: " << tol_byte << " T: " << now_time - last_time << " bw: " << bw << " Mbps" << endl;
				f.close();
				if (count_num == 30) {
					exit(0);
				}
				last_time = 0;
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
