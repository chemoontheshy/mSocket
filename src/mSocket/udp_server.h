#ifndef  UDP_SERVER_H
#define  UDP_SERVER_H
#pragma once
#include<iostream>
#include <stdio.h>
#include<chrono>
#include <winsock2.h>
#include <WS2tcpip.h>

/// <summary>
/// TCP客服端封装
/// </summary>
class SUDP
{
public:
	/// <summary>
	/// 构造函数
	/// </summary>
	explicit SUDP();
	/// <summary>
	/// 析构函数
	/// </summary>
	~SUDP();

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
	static bool tcpSockInit(SOCKET sock, const char* ip, uint16_t port, size_t ListenQueue = 5);
	/// <summary>
	/// 结束，释放内存
	/// </summary>
	static void free();


};
#endif