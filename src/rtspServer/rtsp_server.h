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
constexpr size_t BUF_MAX_SIZE = (1 << 20);


constexpr size_t MAX_SIZE = 2048;

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
	static void sockFree();

private:
	/// <summary>
	/// �ظ�OPTIONS����
	/// </summary>
	/// <param name="buffer">������������ݵĻ�������ַ��ͷָ��</param>
	/// <param name="bufferLen">�ɽ��յ�����ֽ��������ܳ���buf�������Ĵ�С��</param>
	/// <param name="cseq">������������к�</param>
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
	/// �ظ�DESCRBE����
	/// </summary>
	/// <param name="buffer">������������ݵĻ�������ַ��ͷָ��</param>
	/// <param name="bufferLen">�ɽ��յ�����ֽ��������ܳ���buf�������Ĵ�С��</param>
	/// <param name="cseq">������������к�</param>
	/// <param name="url">���ŵ�url</param>
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
	/// �ظ�SETUP����
	/// </summary>
	/// <param name="buffer">������������ݵĻ�������ַ��ͷָ��</param>
	/// <param name="bufferLen">�ɽ��յ�����ֽ��������ܳ���buf�������Ĵ�С��</param>
	/// <param name="cseq">������������к�</param>
	/// <param name="clientRTP_Port">�ͻ����ṩ��RTP���</param>
	/// <param name="ssrcNum">ͬ����Դ��ʶ�������ڱ�ʶͬ����Դ</param>
	/// <param name="sessionID">��ʶ�ỰID�������Զ��壩</param>
	/// <param name="timeout">��ʱʱ��</param>
	static void replayCmd_SETUP(char* buffer, const int bufferLen, const int cseq, const int clientRTP_Port, const int ssrcNum, const char* sessionID, const int timeout)
	{
		snprintf(buffer, bufferLen, "RTSP/1.0 200 OK\r\nCseq: %d\r\nTransport: RTP/AVP;unicast;client_port=%d-%d;server_port=%d-%d;ssrc=%d;mode=play\r\nSession: %s; timeout=%d\r\n\r\n",
			cseq, clientRTP_Port, clientRTP_Port + 1, SERVER_RTP_PORT, SERVER_RTCP_PORT, ssrcNum, sessionID, timeout);
	}

	/// <summary>
	/// �ظ�PLAY����
	/// </summary>
	/// <param name="buffer">������������ݵĻ�������ַ��ͷָ��</param>
	/// <param name="bufferLen">�ɽ��յ�����ֽ��������ܳ���buf�������Ĵ�С��</param>
	/// <param name="cseq">������������к�</param>
	/// <param name="sessionID">��ʶ�ỰID�������Զ��壩</param>
	/// <param name="timeout">��ʱʱ��</param>
	static void replyCmd_PLAY(char* buffer, const int bufferLen, const int cseq, const char* sessionID, const int timeout)
	{
		snprintf(buffer, bufferLen, "RTSP/1.0 200 OK\r\nCseq: %d\r\nRange: npt=0.000-\r\nSession: %s; timeout=%d\r\n\r\n", cseq, sessionID, timeout);

	}

	/// <summary>
	/// �ظ�TEARDOWN����
	/// </summary>
	/// <param name="buffer">������������ݵĻ�������ַ��ͷָ��</param>
	/// <param name="bufferLen">�ɽ��յ�����ֽ��������ܳ���buf�������Ĵ�С��</param>
	/// <param name="cseq">������������к�</param>
	static void replyCmd_TEARDOWN(char* buffer, const int bufferLen, const int cseq)
	{
		snprintf(buffer, bufferLen, "RTSP/1.0 200 OK\r\nCseq: %d\r\n\r\n", cseq);
	}

	/// <summary>
	/// RTSP�Ự
	/// </summary>
	/// <param name="serverSockfd">TCP�Ự</param>
	/// <param name="clientRtpSockfd">���ڽ�������ͷ������ͨ��(UDP��)</param>
	/// <param name="serverRtpSockfd">���ڴ���RTP��ͨ��</param>
	/// <param name="serverRtcpSockfd">���ڴ���RTCP��ͨ��</param>
	static bool serveClient(SOCKET serverSockfd, SOCKET clientRtpSockfd, SOCKET serverRtpSockfd,
		SOCKET serverRtcpSockfd);
	
	/// <summary>
	/// ָ��ĳ�ָ�ʽ���ı����ַ�����ת����ĳ�����ݽṹ�Ĺ���
	/// </summary>
	/// <param name="src">������ı�</param>
	/// <param name="line">һ�еĻ�����</param>
	/// <returns></returns>
	static char* lineParser(char* src, char* line);

	/// <summary>
	/// ת��UDP��
	/// </summary>
	/// <param name="clientRtpSockfd">���ڽ�������ͷ������ͨ��(UDP��)</param>
	/// <param name="serverRtpSockfd">���ڴ���RTP��ͨ��</param>
	/// <param name="serverRtcpSockfd">���ڴ���RTCP��ͨ��</param>
	static void thread_do(SOCKET clientRtpSockfd, SOCKET serverRtpSockfd, const int serverRtpPort);

};
#endif

