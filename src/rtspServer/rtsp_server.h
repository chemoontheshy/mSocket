#ifndef  RTSP_SERVER_H
#define  RTSP_SERVER_H
#pragma once
#include<iostream>
#include <stdio.h>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <string.h>

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
constexpr size_t maxBufferSize = (1 << 21);


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
	static void free();

private:
	/// <summary>
	/// 回复OPTIONS请求
	/// </summary>
	/// <param name="buffer">填充的地址头</param>
	/// <param name="bufferLen"></param>
	/// <param name="cseq"></param>
	static void replayCmd_OPTIONS(char* buffer, const int bufferLen, const int cseq)
	{
		snprintf(buffer, bufferLen, "RTSP/1.0 200 OK\r\n"
			"CSeq: %d\r\n"
			"Public: OPTIONS, DESCRIBE, SETUP, PLAY\r\n"
			"\r\n",
			cseq);
	}

	//指把某种格式的文本（字符串）转换成某种数据结构的过程
	static char* lineParser(char* src, char* line);
};
#endif

