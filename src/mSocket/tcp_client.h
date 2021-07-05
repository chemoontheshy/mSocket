#ifndef  CLIENT_H
#define  CLIENT_H
#pragma once
#include<iostream>
#include <stdio.h>
#include <winsock2.h>
#include <WS2tcpip.h>

/// <summary>
/// TCP�ͷ��˷�װ
/// </summary>
class CTCP
{
public:
	/// <summary>
	/// ���캯��
	/// </summary>
	explicit CTCP();
	/// <summary>
	/// ��������
	/// </summary>
	~CTCP();

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
	static bool Connect(SOCKET sock, const char* ip, uint16_t port);

	/// <summary>
	/// �������ͷ��ڴ�
	/// </summary>
	static void free();


};
#endif