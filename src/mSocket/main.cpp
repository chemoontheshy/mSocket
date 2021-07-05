#include "tcp_server.h"
#include "tcp_client.h"
#include "udp_server.h"
#include "rtsp_server.h"

int main(int argc, char* argv[])
{
	/*STCP tcpSerVer;
	tcpSerVer.start();*/
	/*CTCP tcpClient;
	tcpClient.start();*/
	RTSP rtspServer;
	rtspServer.start();
}