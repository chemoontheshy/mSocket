#include "get_socket_data.h"


vsnc::vget::VSocket::VSocket(const size_t port)
{
	// 3.socket部分
	// socket初始化
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		exit(-1);
	}
	// 接受数据部分
	m_iSocktFD = socket(AF_INET, SOCK_DGRAM, 0);
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(static_cast<u_short>(port));
	((struct sockaddr_in&)addr).sin_addr.s_addr = INADDR_ANY;
	if (bind(m_iSocktFD, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr_in)) < 0)
	{
		std::cout << "UDP::bind() failed." << std::endl;
		exit(-1);
	}
	// 接收数据包缓存区
	m_cRBuf = new char[1500];
	m_iClientLen = sizeof(sockaddr_in);

	// 设置两帧的socket接收的缓冲区
	int rcvBufSize = 1024 * 1024 * 8;
	socklen_t optLen = sizeof(rcvBufSize);
	if (setsockopt(m_iSocktFD, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<const char*>(&rcvBufSize), optLen) < 0) 
	{
		std::cout << "setsockopt failed." << std::endl;
		exit(-1);
	}
}

vsnc::vget::VSocket::~VSocket()
{
	delete[] m_cRBuf;
	closesocket(m_iSocktFD);
}

bool vsnc::vget::VSocket::getData(uint8_t* ptr, const size_t& size)
{
	size_t tempSize = 0;
	while (true)
	{
		if ((tempSize + 1500) > size)
		{
			return true;
		}
		auto size = recvfrom(m_iSocktFD, m_cRBuf, 1500, 0, reinterpret_cast<sockaddr*>(&m_iClient), &m_iClientLen);
		if (size > 0) 
		{
			memcpy(ptr + tempSize, m_cRBuf,size);
			tempSize += size;
		}
	}
	return false;
}

