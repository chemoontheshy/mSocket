#include "rtsp_server.h"

int main(int argc, char* argv[])
{
	RTSP rtspServer;
	rtspServer.start(8002);
}
