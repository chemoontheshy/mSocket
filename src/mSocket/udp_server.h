#ifndef  UDP_SERVER_H
#define  UDP_SERVER_H
#pragma once
#include<iostream>
#include <stdio.h>
#include<chrono>
#include <winsock2.h>
#include <WS2tcpip.h>

/// <summary>
/// TCP�ͷ��˷�װ
/// </summary>
class SUDP
{
public:
	/// <summary>
	/// ���캯��
	/// </summary>
	explicit SUDP();
	/// <summary>
	/// ��������
	/// </summary>
	~SUDP();

	void start();

private:
	/// <summary>
	/// �����׽���
	/// </summary>
	/// <param name="af">IP��ַ���ͣ����õ���AF_INET��AF_INET6��PF_INET�ȼ���AF_INET</param>
	/// <param name="type">���ִ��䷽ʽ��������SOCK_STREAM(����ʽ�׽���/�������ӵ��׽���)��
	/// SOCK_DGRAM(���ݱ��׽���/�����ӵ��׽���)</param>
	/// <param name="protocol">����Э�飻���õ���IPPROTO_TCP��IPPTOTO_UDP���ֱ��ʾTCP����Э���UDP����Э��</param>
	/// <returns>��������>0,���󷵻�-1��INVALID_SOCKET��</returns>
	static SOCKET Socket(int af, int type, int protocol = 0);
	/// <summary>
	/// ���������׽�����IP��ַ���˿ڰ󶨡�
	/// </summary>
	/// <param name="sock">�������׽���</param>
	/// <param name="ip">ip��ַ</param>
	/// <param name="port">�˿�</param>
	/// <returns>��������true,���󷵻�false</returns>
	static bool Bind(SOCKET sock, const char* ip, uint16_t port);
	/// <summary>
	/// ��ʼ��tcp�ͷ��ˣ���ʼ����
	/// </summary>
	/// <param name="sock">�������׽���</param>
	/// <param name="ip">ip��ַ</param>
	/// <param name="port">�˿�</param>
	/// <param name="ListenQueue">����������</param>
	/// <returns>��������true,���󷵻�false</returns>
	static bool tcpSockInit(SOCKET sock, const char* ip, uint16_t port, size_t ListenQueue = 5);
	/// <summary>
	/// �������ͷ��ڴ�
	/// </summary>
	static void free();


};
#endif