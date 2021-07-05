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
constexpr uint16_t SERVER_RTP_PORT = 4001;
constexpr uint16_t SERVER_RTCP_PORT = SERVER_RTP_PORT + 1;
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
	static bool rtspSockInit(SOCKET sock, const char* ip, uint16_t port, int ListenQueue = 5);
	/// <summary>
	/// �������ͷ��ڴ�
	/// </summary>
	static void free();

private:
	int cliRtpPort{ -1 }, cliRtcpPort{ -1 };
	//reply
	//OPTIONS
	static void replyCmd_OPTIONS(char* buffer, const int bufferLen, const int cseq)
	{
		snprintf(buffer, bufferLen, "RTSP/1.0 200 OK\r\nCseq: %d\r\nPubilic: OPTIONS, DESCRIBE, SEPUT, PLAY\r\n\r\n", cseq);
	}
	static void replayCmd_SETUP(char* buffer, const int bufferLen, const int cseq, const int clientRTP_Port, const int ssrcNum, const char* sessionID, const int timeout)
	{
		snprintf(buffer, bufferLen, "RTSP/1.0 200 OK\r\nCseq: %d\r\nTransport: RTP/AVP;unicast;client_port=%d-%d;server_port=%d-%d;ssrc=%d;mode=play\r\nSession: %s; timeout=%d\r\n\r\n",
			cseq, clientRTP_Port, clientRTP_Port + 1, SERVER_RTP_PORT, SERVER_RTCP_PORT, ssrcNum, sessionID, timeout);
	}
	static void replayCmd_PLAY(char* buffer, const int bufferLen, const int cseq, const char* sessionID, const int timeout)
	{

	}
	static void replyCmd_DESCRBE(char* buffer, const int bufferLen, const int cseq, const char* url)
	{
		char ip[100]{ 0 };
		char sdp[500]{ 0 };
		sscanf_s(url, "rtsp://%[^:]:", ip, 100);
		snprintf(sdp, sizeof(sdp), "v=0\r\no=- %lld 1 IN IP4 %s\r\nt=0 0\r\na=control:*\r\nm=video 0 RTP/AVP 96\r\na=rtpmap:96 H264/90000\r\na=control:track0\r\n", time(nullptr), ip);
		snprintf(buffer, bufferLen, "RTSP/1.0 200 OK\r\nCseq: %d\r\nContent-Base: %s\r\nContent-type: application/sdp\r\nContent-length: %lld\r\n\r\n%s", cseq, url, sizeof(sdp), sdp);
	}
	
	//ָ��ĳ�ָ�ʽ���ı����ַ�����ת����ĳ�����ݽṹ�Ĺ���
	static char* lineParser(char* src, char* line);


};
#endif
