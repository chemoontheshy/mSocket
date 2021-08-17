#ifndef  SENDER_H
#define  SENDER_H
#pragma once
#include<iostream>
#include <stdio.h>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <string.h>
#include <thread>
#include <chrono>
//ʱ���
#include"utils.h"

using namespace std;
using namespace chrono;



/// <summary>
/// ָ��UDP���͵Ķ˿�
/// </summary>
constexpr uint16_t SENDER_UDP_PORT = 55532;

/// <summary>
/// RTSP�ͻ��˷�װ
/// </summary>
class Sender
{
public:
	/// <summary>
	/// ���캯��
	/// </summary>
	explicit Sender();
	/// <summary>
	/// ��������
	/// </summary>
	~Sender();

	/// <summary>
	/// ��ʼ��Ŀ�귢������
	/// </summary>
	/// <param name="Port">Ŀ��Ķ˿�</param>
	/// <param name="IP">Ŀ��ĵ�ַ</param>
	void start(uint16_t Port,const char *IP);

	/// <summary>
	/// ������������Port��UDP��
	/// </summary>
	/// <param name="Port">�������ݵĶ˿�</param>
	void read(uint16_t Port);

	float delaySend(uint16_t Port, const char* IP);

	void delayReply(uint16_t Port);

private:
	/// <summary>
	/// ����TCPSocket()
	/// </summary>
	/// <returns>����һ��socket</returns>
	static SOCKET  creatTcpSocket();

	/// <summary>
	/// ����TCPSocket()
	/// </summary>
	/// <returns>����һ��socket</returns>
	static SOCKET  creatUdpSocket();
	/// <summary>
	/// ���������׽�����IP��ַ���˿ڰ󶨡�
	/// </summary>
	/// <param name="sock">�������׽���</param>
	/// <param name="ip">ip��ַ</param>
	/// <param name="port">�˿�</param>
	/// <returns>��������true,���󷵻�false</returns>
	static bool bindSocket(SOCKET sock, const char* ip, uint16_t port);
	/// <summary>
	/// ��ʼ��tcp�ͷ��ˣ���ʼ����,u
	/// </summary>
	/// <param name="sock">�������׽���</param>
	/// <param name="ip">ip��ַ</param>
	/// <param name="port">�˿�</param>
	/// <param name="ListenQueue">����������</param>
	/// <returns>��������true,���󷵻�false</returns>
	static bool sockInit(SOCKET sock, const char* ip, uint16_t port, int ListenQueue = 5);

	/// <summary>
	/// �����ͷ��ڴ�
	/// </summary>
	static void sockFree();

private:
	

};
#endif

