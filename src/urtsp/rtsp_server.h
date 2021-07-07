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
constexpr uint16_t SERVER_RTP_PORT = 4002;
constexpr uint16_t SERVER_RTCP_PORT = SERVER_RTP_PORT + 1;
constexpr size_t maxBufferSize = (1 << 21);
/// <summary>
/// TCP�ͷ��˷�װ
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

	void start();

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
	static bool bind(SOCKET sock, const char* ip, uint16_t port);
	/// <summary>
	/// ��ʼ��tcp�ͷ��ˣ���ʼ����
	/// </summary>
	/// <param name="sock">�������׽���</param>
	/// <param name="ip">ip��ַ</param>
	/// <param name="port">�˿�</param>
	/// <param name="ListenQueue">����������</param>
	/// <returns>��������true,���󷵻�false</returns>
	static bool sockInit(SOCKET sock, const char* ip, uint16_t port, int ListenQueue = 5);

	static int acceptClient
	/// <summary>
	/// �������ͷ��ڴ�
	/// </summary>
	static void free();

private:
	SOCKET servRtspSockfd;
	SOCKET servRtpSockfd;
	int cliRtpPort{ -1 }, cliRtcpPort{ -1 };
	//reply
	//OPTIONS
	static void replyCmd_OPTIONS(char* buffer, const int bufferLen, const int cseq)
	{
		snprintf(buffer, bufferLen, "RTSP/1.0 200 OK\r\n"
			"CSeq: %d\r\n"
			"Public: OPTIONS, DESCRIBE, SETUP, PLAY\r\n"
			"\r\n",
			cseq);
	}
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
	static void replayCmd_SETUP(char* buffer, const int bufferLen, const int cseq, const int clientRTP_Port, const int ssrcNum, const char* sessionID, const int timeout)
	{
		snprintf(buffer, bufferLen, "RTSP/1.0 200 OK\r\nCseq: %d\r\nTransport: RTP/AVP;unicast;client_port=%d-%d;server_port=%d-%d;ssrc=%d;mode=play\r\nSession: %s; timeout=%d\r\n\r\n",
			cseq, clientRTP_Port, clientRTP_Port + 1, SERVER_RTP_PORT, SERVER_RTCP_PORT, ssrcNum, sessionID, timeout);
	}
	static void replyCmd_PLAY(char* buffer, const int bufferLen, const int cseq, const char* sessionID, const int timeout)
	{
		snprintf(buffer, bufferLen, "RTSP/1.0 200 OK\r\nCseq: %d\r\nRange: npt=0.000-\r\nSession: %s; timeout=%d\r\n\r\n", cseq, sessionID, timeout);

	}


	//ָ��ĳ�ָ�ʽ���ı����ַ�����ת����ĳ�����ݽṹ�Ĺ���
	static char* lineParser(char* src, char* line);

	void serveClient(SOCKET clientfd, const sockaddr_in& cliAddr, SOCKET rtpFD, int ssrcNum, const char* sessionID, int timeout, float fps);
};
#endif
