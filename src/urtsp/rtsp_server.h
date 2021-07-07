#ifndef  RTSP_SERVER_H
#define  RTSP_SERVER_H
#pragma once
#include<iostream>
#include <stdio.h>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <string.h>

//RTSP
constexpr uint16_t SERVER_RTSP_PORT = 8554;
//RTP
constexpr uint16_t SERVER_RTP_PORT = 4002;
constexpr uint16_t SERVER_RTCP_PORT = SERVER_RTP_PORT + 1;
constexpr size_t maxBufferSize = (1 << 21);
/// <summary>
/// TCP客服端封装
/// </summary>
class RTSP
{
public:
	/// <summary>
	/// 构造函数
	/// </summary>
	explicit RTSP();
	/// <summary>
	/// 析构函数
	/// </summary>
	~RTSP();

	void start();

private:
	/// <summary>
	/// 创建TCPSocket()
	/// </summary>
	/// <returns>返回一个socket</returns>
	static SOCKET  creatTcpSocket();

	/// <summary>
	/// 创建TCPSocket()
	/// </summary>
	/// <returns>返回一个socket</returns>
	static SOCKET  creatUdpSocket();
	/// <summary>
	/// 将创建的套接字与IP地址、端口绑定。
	/// </summary>
	/// <param name="sock">创建的套接字</param>
	/// <param name="ip">ip地址</param>
	/// <param name="port">端口</param>
	/// <returns>正常返回true,错误返回false</returns>
	static bool bind(SOCKET sock, const char* ip, uint16_t port);
	/// <summary>
	/// 初始化tcp客服端，开始监听
	/// </summary>
	/// <param name="sock">创建的套接字</param>
	/// <param name="ip">ip地址</param>
	/// <param name="port">端口</param>
	/// <param name="ListenQueue">最大的连接数</param>
	/// <returns>正常返回true,错误返回false</returns>
	static bool sockInit(SOCKET sock, const char* ip, uint16_t port, int ListenQueue = 5);

	static int acceptClient
	/// <summary>
	/// 结束，释放内存
	/// </summary>
	static void free();

private:
	SOCKET servRtspSockfd;
	SOCKET servRtpSockfd;
	int cliRtpPort{ -1 }, cliRtcpPort{ -1 };
	//reply
	//OPTIONS
	static void replyCmd_OPTIONS(char* buffer, const int bufferLen, const int cseq)
	{
		snprintf(buffer, bufferLen, "RTSP/1.0 200 OK\r\n"
			"CSeq: %d\r\n"
			"Public: OPTIONS, DESCRIBE, SETUP, PLAY\r\n"
			"\r\n",
			cseq);
	}
	static void replyCmd_DESCRBE(char* buffer, const int bufferLen, const int cseq, const char* url)
	{
		char ip[100]{ 0 };
		char sdp[500]{ 0 };
		sscanf_s(url, "rtsp://%[^:]:", ip, 100);
		snprintf(sdp, sizeof(sdp), "v=0\r\n"
			"o=- 9%lld 1 IN IP4 %s\r\n"
			"t=0 0\r\n"
			"a=control:*\r\n"
			"m=video 0 RTP/AVP 96\r\n"
			"a=rtpmap:96 H264/90000\r\n"
			"a=control:track0\r\n",
			time(NULL), ip);

		snprintf(buffer, bufferLen, "RTSP/1.0 200 OK\r\nCSeq: %d\r\n"
			"Content-Base: %s\r\n"
			"Content-type: application/sdp\r\n"
			"Content-length: %lld\r\n\r\n"
			"%s",
			cseq,
			url,
			strlen(sdp),
			sdp);
	}
	static void replayCmd_SETUP(char* buffer, const int bufferLen, const int cseq, const int clientRTP_Port, const int ssrcNum, const char* sessionID, const int timeout)
	{
		snprintf(buffer, bufferLen, "RTSP/1.0 200 OK\r\nCseq: %d\r\nTransport: RTP/AVP;unicast;client_port=%d-%d;server_port=%d-%d;ssrc=%d;mode=play\r\nSession: %s; timeout=%d\r\n\r\n",
			cseq, clientRTP_Port, clientRTP_Port + 1, SERVER_RTP_PORT, SERVER_RTCP_PORT, ssrcNum, sessionID, timeout);
	}
	static void replyCmd_PLAY(char* buffer, const int bufferLen, const int cseq, const char* sessionID, const int timeout)
	{
		snprintf(buffer, bufferLen, "RTSP/1.0 200 OK\r\nCseq: %d\r\nRange: npt=0.000-\r\nSession: %s; timeout=%d\r\n\r\n", cseq, sessionID, timeout);

	}


	//指把某种格式的文本（字符串）转换成某种数据结构的过程
	static char* lineParser(char* src, char* line);

	void serveClient(SOCKET clientfd, const sockaddr_in& cliAddr, SOCKET rtpFD, int ssrcNum, const char* sessionID, int timeout, float fps);
};
#endif
