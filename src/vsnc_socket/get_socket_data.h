#pragma once
#include<iostream>
#include <winsock2.h>
#include <WS2tcpip.h>

namespace vsnc
{
	namespace vget
	{
		class VSocket
		{
		public:
			VSocket(const size_t port);
			~VSocket();

			bool getData(uint8_t* ptr,const size_t& size);
		private:
			SOCKET        m_iSocktFD;
			char*         m_cRBuf;
			sockaddr_in   m_iClient;
			socklen_t     m_iClientLen;
		};
	}
}