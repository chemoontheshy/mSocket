#ifndef  RTSP_SERVER_H
#define  RTSP_SERVER_H
#pragma once
#include<iostream>
#include <stdio.h>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <string.h>

/// <summary>
/// ָ��RTSP��ַ�Ķ˿�
/// </summary>
constexpr uint16_t SERVER_PORT = 8554;

/// <summary>
/// ��������ͷ������壩���͹�����������ָ���˿�
/// </summary>
constexpr uint16_t CLIENT_RTP_PORT = 4002;

/// <summary>
/// ָ��RTP���͵Ķ˿�
/// </summary>
constexpr uint16_t SERVER_RTP_PORT = 55532;

/// <summary>
/// ָ��RTCP���͵Ķ˿�
/// </summary>
constexpr uint16_t SERVER_RTCP_PORT = SERVER_RTP_PORT + 1;

/// <summary>
/// ����Buffer����size
/// </summary>
constexpr size_t maxBufferSize = (1 << 21);


/// <summary>
/// RTSP�ͻ��˷�װ
/// </summary>
class RTSP
{
public:
	/// <summary>
	/// ���캯��
	/// </summary>
	explicit RTSP();
	/// <summary>
	/// ��������
	/// </summary>
	~RTSP();

	void start(uint16_t clientRtpPort);

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
	static void free();

private:
	/// <summary>
	/// �ظ�OPTIONS����
	/// </summary>
	/// <param name="buffer">���ĵ�ַͷ</param>
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

	//ָ��ĳ�ָ�ʽ���ı����ַ�����ת����ĳ�����ݽṹ�Ĺ���
	static char* lineParser(char* src, char* line);
};
#endif

