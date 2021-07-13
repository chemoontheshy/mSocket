#ifndef  RTSP_SERVER_H
#define  RTSP_SERVER_H
#pragma once
#include<iostream>
#include <stdio.h>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <string.h>
#include <thread>

/// <summary>
/// 指定RTSP地址的端口
/// </summary>
constexpr uint16_t SERVER_PORT = 8554;

/// <summary>
/// 接收摄像头（编码板）发送过来的码流的指定端口
/// </summary>
constexpr uint16_t CLIENT_RTP_PORT = 4002;

/// <summary>
/// 指定RTP发送的端口
/// </summary>
constexpr uint16_t SERVER_RTP_PORT = 55532;

/// <summary>
/// 指定RTCP发送的端口
/// </summary>
constexpr uint16_t SERVER_RTCP_PORT = SERVER_RTP_PORT + 1;

/// <summary>
/// 接收Buffer最大的size
/// </summary>
constexpr size_t BUF_MAX_SIZE = (1 << 20);


constexpr size_t MAX_SIZE = 2048;

/// <summary>
/// RTSP客户端封装
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

	void start(uint16_t clientRtpPort);

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
	static bool bindSocket(SOCKET sock, const char* ip, uint16_t port);
	/// <summary>
	/// 初始化tcp客服端，开始监听,u
	/// </summary>
	/// <param name="sock">创建的套接字</param>
	/// <param name="ip">ip地址</param>
	/// <param name="port">端口</param>
	/// <param name="ListenQueue">最大的连接数</param>
	/// <returns>正常返回true,错误返回false</returns>
	static bool sockInit(SOCKET sock, const char* ip, uint16_t port, int ListenQueue = 5);

	/// <summary>
	/// 结束释放内存
	/// </summary>
	static void sockFree();

private:
	/// <summary>
	/// 回复OPTIONS请求
	/// </summary>
	/// <param name="buffer">保存待传输数据的缓冲区地址的头指针</param>
	/// <param name="bufferLen">可接收的最大字节数（不能超过buf缓冲区的大小）</param>
	/// <param name="cseq">本次请求的序列号</param>
	static void replyCmd_OPTIONS(char* buffer, const int bufferLen, const int cseq)
	{
		printf("bufferLen%d\n",bufferLen);
		snprintf(buffer, bufferLen, "RTSP/1.0 200 OK\r\n"
			"CSeq: %d\r\n"
			"Public: OPTIONS, DESCRIBE, SETUP, PLAY,TEARDOWN \r\n"
			"\r\n",
			cseq);
	}

	/// <summary>
	/// 回复DESCRBE请求
	/// </summary>
	/// <param name="buffer">保存待传输数据的缓冲区地址的头指针</param>
	/// <param name="bufferLen">可接收的最大字节数（不能超过buf缓冲区的大小）</param>
	/// <param name="cseq">本次请求的序列号</param>
	/// <param name="url">播放的url</param>
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

	/// <summary>
	/// 回复SETUP请求
	/// </summary>
	/// <param name="buffer">保存待传输数据的缓冲区地址的头指针</param>
	/// <param name="bufferLen">可接收的最大字节数（不能超过buf缓冲区的大小）</param>
	/// <param name="cseq">本次请求的序列号</param>
	/// <param name="clientRTP_Port">客户端提供的RTP入口</param>
	/// <param name="ssrcNum">同步信源标识符：用于标识同步信源</param>
	/// <param name="sessionID">标识会话ID（可以自定义）</param>
	/// <param name="timeout">超时时间</param>
	static void replayCmd_SETUP(char* buffer, const int bufferLen, const int cseq, const int clientRTP_Port, const int ssrcNum, const char* sessionID, const int timeout)
	{
		snprintf(buffer, bufferLen, "RTSP/1.0 200 OK\r\nCseq: %d\r\nTransport: RTP/AVP;unicast;client_port=%d-%d;server_port=%d-%d;ssrc=%d;mode=play\r\nSession: %s; timeout=%d\r\n\r\n",
			cseq, clientRTP_Port, clientRTP_Port + 1, SERVER_RTP_PORT, SERVER_RTCP_PORT, ssrcNum, sessionID, timeout);
	}

	/// <summary>
	/// 回复PLAY请求
	/// </summary>
	/// <param name="buffer">保存待传输数据的缓冲区地址的头指针</param>
	/// <param name="bufferLen">可接收的最大字节数（不能超过buf缓冲区的大小）</param>
	/// <param name="cseq">本次请求的序列号</param>
	/// <param name="sessionID">标识会话ID（可以自定义）</param>
	/// <param name="timeout">超时时间</param>
	static void replyCmd_PLAY(char* buffer, const int bufferLen, const int cseq, const char* sessionID, const int timeout)
	{
		snprintf(buffer, bufferLen, "RTSP/1.0 200 OK\r\nCseq: %d\r\nRange: npt=0.000-\r\nSession: %s; timeout=%d\r\n\r\n", cseq, sessionID, timeout);

	}

	/// <summary>
	/// 回复TEARDOWN请求
	/// </summary>
	/// <param name="buffer">保存待传输数据的缓冲区地址的头指针</param>
	/// <param name="bufferLen">可接收的最大字节数（不能超过buf缓冲区的大小）</param>
	/// <param name="cseq">本次请求的序列号</param>
	static void replyCmd_TEARDOWN(char* buffer, const int bufferLen, const int cseq)
	{
		snprintf(buffer, bufferLen, "RTSP/1.0 200 OK\r\nCseq: %d\r\n\r\n", cseq);
	}

	/// <summary>
	/// RTSP会话
	/// </summary>
	/// <param name="serverSockfd">TCP会话</param>
	/// <param name="clientRtpSockfd">用于接收摄像头码流的通道(UDP包)</param>
	/// <param name="serverRtpSockfd">用于传输RTP的通道</param>
	/// <param name="serverRtcpSockfd">用于传输RTCP的通道</param>
	static bool serveClient(SOCKET serverSockfd, SOCKET clientRtpSockfd, SOCKET serverRtpSockfd,
		SOCKET serverRtcpSockfd);
	
	/// <summary>
	/// 指把某种格式的文本（字符串）转换成某种数据结构的过程
	/// </summary>
	/// <param name="src">输入的文本</param>
	/// <param name="line">一行的缓冲区</param>
	/// <returns></returns>
	static char* lineParser(char* src, char* line);

	/// <summary>
	/// 转发UDP包
	/// </summary>
	/// <param name="clientRtpSockfd">用于接收摄像头码流的通道(UDP包)</param>
	/// <param name="serverRtpSockfd">用于传输RTP的通道</param>
	/// <param name="serverRtcpSockfd">用于传输RTCP的通道</param>
	static void thread_do(SOCKET clientRtpSockfd, SOCKET serverRtpSockfd, const int serverRtpPort);

};
#endif

