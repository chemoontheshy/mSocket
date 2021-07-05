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
constexpr uint16_t SERVER_RTP_PORT = 4001;
constexpr uint16_t SERVER_RTCP_PORT = SERVER_RTP_PORT + 1;
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
	/// 创建套接字
	/// </summary>
	/// <param name="af">IP地址类型；常用的有AF_INET和AF_INET6，PF_INET等价于AF_INET</param>
	/// <param name="type">数字传输方式；常用有SOCK_STREAM(流格式套接字/面向连接的套接字)和
	/// SOCK_DGRAM(数据报套接字/无连接的套接字)</param>
	/// <param name="protocol">传输协议；常用的有IPPROTO_TCP和IPPTOTO_UDP，分别表示TCP传输协议和UDP传输协议</param>
	/// <returns>正常返回>0,错误返回-1（INVALID_SOCKET）</returns>
	static SOCKET Socket(int af, int type, int protocol = 0);
	/// <summary>
	/// 将创建的套接字与IP地址、端口绑定。
	/// </summary>
	/// <param name="sock">创建的套接字</param>
	/// <param name="ip">ip地址</param>
	/// <param name="port">端口</param>
	/// <returns>正常返回true,错误返回false</returns>
	static bool Bind(SOCKET sock, const char* ip, uint16_t port);
	/// <summary>
	/// 初始化tcp客服端，开始监听
	/// </summary>
	/// <param name="sock">创建的套接字</param>
	/// <param name="ip">ip地址</param>
	/// <param name="port">端口</param>
	/// <param name="ListenQueue">最大的连接数</param>
	/// <returns>正常返回true,错误返回false</returns>
	static bool rtspSockInit(SOCKET sock, const char* ip, uint16_t port, int ListenQueue = 5);
	/// <summary>
	/// 结束，释放内存
	/// </summary>
	static void free();

private:
	int cliRtpPort{ -1 }, cliRtcpPort{ -1 };
	//reply
	//OPTIONS
	static void replyCmd_OPTIONS(char* buffer, const int bufferLen, const int cseq)
	{
		snprintf(buffer, bufferLen, "RTSP/1.0 200 OK\r\nCseq: %d\r\nPubilic: OPTIONS, DESCRIBE, SEPUT, PLAY\r\n\r\n", cseq);
	}
	static void replayCmd_SETUP(char* buffer, const int bufferLen, const int cseq, const int clientRTP_Port, const int ssrcNum, const char* sessionID, const int timeout)
	{
		snprintf(buffer, bufferLen, "RTSP/1.0 200 OK\r\nCseq: %d\r\nTransport: RTP/AVP;unicast;client_port=%d-%d;server_port=%d-%d;ssrc=%d;mode=play\r\nSession: %s; timeout=%d\r\n\r\n",
			cseq, clientRTP_Port, clientRTP_Port + 1, SERVER_RTP_PORT, SERVER_RTCP_PORT, ssrcNum, sessionID, timeout);
	}
	static void replayCmd_PLAY(char* buffer, const int bufferLen, const int cseq, const char* sessionID, const int timeout)
	{

	}
	static void replyCmd_DESCRBE(char* buffer, const int bufferLen, const int cseq, const char* url)
	{
		char ip[100]{ 0 };
		char sdp[500]{ 0 };
		sscanf_s(url, "rtsp://%[^:]:", ip, 100);
		snprintf(sdp, sizeof(sdp), "v=0\r\no=- %lld 1 IN IP4 %s\r\nt=0 0\r\na=control:*\r\nm=video 0 RTP/AVP 96\r\na=rtpmap:96 H264/90000\r\na=control:track0\r\n", time(nullptr), ip);
		snprintf(buffer, bufferLen, "RTSP/1.0 200 OK\r\nCseq: %d\r\nContent-Base: %s\r\nContent-type: application/sdp\r\nContent-length: %lld\r\n\r\n%s", cseq, url, sizeof(sdp), sdp);
	}
	
	//指把某种格式的文本（字符串）转换成某种数据结构的过程
	static char* lineParser(char* src, char* line);


};
#endif
