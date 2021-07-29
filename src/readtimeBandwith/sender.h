#ifndef  SENDER_H
#define  SENDER_H
#pragma once
#include<iostream>
#include <stdio.h>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <string.h>
#include <thread>


/// <summary>
/// 指定UDP发送的端口
/// </summary>
constexpr uint16_t SENDER_UDP_PORT = 55532;

/// <summary>
/// RTSP客户端封装
/// </summary>
class Sender
{
public:
	/// <summary>
	/// 构造函数
	/// </summary>
	explicit Sender();
	/// <summary>
	/// 析构函数
	/// </summary>
	~Sender();

	/// <summary>
	/// 开始往目标发送数据
	/// </summary>
	/// <param name="Port">目标的端口</param>
	/// <param name="IP">目标的地址</param>
	void start(uint16_t Port,const char *IP);

	void read(uint16_t Port);

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
	

};
#endif

