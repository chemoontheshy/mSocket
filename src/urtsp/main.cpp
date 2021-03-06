/*
* ???ߣ?_JT_
* ???ͣ?https://blog.csdn.net/weixin_42462202
*/

#include<iostream>
#include <stdio.h>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <string.h>
#include <time.h>

#define SERVER_PORT     8554
#define SERVER_RTP_PORT  85552
#define SERVER_RTCP_PORT 85553
#define BUF_MAX_SIZE    (1024*1024)

static int createTcpSocket()
{
    int sockfd;
    int on = 1;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        return -1;

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof(on));

    return sockfd;
}

static int createUdpSocket()
{
    int sockfd;
    int on = 1;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        return -1;

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof(on));

    return sockfd;
}

static int bindSocketAddr(int sockfd, const char* ip, int port)
{
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr)) < 0)
        return -1;

    return 0;
}

static int acceptClient(int sockfd, char* ip, int* port)
{
    int clientfd;
    socklen_t len = 0;
    struct sockaddr_in addr;

    memset(&addr, 0, sizeof(addr));
    len = sizeof(addr);

    clientfd = accept(sockfd, (struct sockaddr*)&addr, &len);
    if (clientfd < 0)
        return -1;

    strcpy(ip, inet_ntoa(addr.sin_addr));
    *port = ntohs(addr.sin_port);

    return clientfd;
}

static char* getLineFromBuf(char* buf, char* line)
{
    while (*buf != '\n')
    {
        *line = *buf;
        line++;
        buf++;
    }

    *line = '\n';
    ++line;
    *line = '\0';

    ++buf;
    return buf;
}

static int handleCmd_OPTIONS(char* result, int cseq)
{
    sprintf(result, "RTSP/1.0 200 OK\r\n"
        "CSeq: %d\r\n"
        "Public: OPTIONS, DESCRIBE, SETUP, PLAY\r\n"
        "\r\n",
        cseq);

    return 0;
}

static int handleCmd_DESCRIBE(char* result, int cseq, char* url)
{
    char sdp[500];
    char localIp[100];

    sscanf(url, "rtsp://%[^:]:", localIp);

    sprintf(sdp, "v=0\r\n"
        "o=- 9%lld 1 IN IP4 %s\r\n"
        "t=0 0\r\n"
        "a=control:*\r\n"
        "m=video 0 RTP/AVP 96\r\n"
        "a=rtpmap:96 H264/90000\r\n"
        "a=control:track0\r\n",
        time(NULL), localIp);

    sprintf(result, "RTSP/1.0 200 OK\r\nCSeq: %d\r\n"
        "Content-Base: %s\r\n"
        "Content-type: application/sdp\r\n"
        "Content-length: %zd\r\n\r\n"
        "%s",
        cseq,
        url,
        strlen(sdp),
        sdp);

    return 0;
}

static int handleCmd_SETUP(char* result, int cseq, int clientRtpPort)
{
    sprintf(result, "RTSP/1.0 200 OK\r\n"
        "CSeq: %d\r\n"
        "Transport: RTP/AVP;unicast;client_port=%d-%d;server_port=%d-%d\r\n"
        "Session: 66334873\r\n"
        "\r\n",
        cseq,
        clientRtpPort,
        clientRtpPort + 1,
        SERVER_RTP_PORT,
        SERVER_RTCP_PORT);

    return 0;
}

static int handleCmd_PLAY(char* result, int cseq)
{
    sprintf(result, "RTSP/1.0 200 OK\r\n"
        "CSeq: %d\r\n"
        "Range: npt=0.000-\r\n"
        "Session: 66334873; timeout=60\r\n\r\n",
        cseq);

    return 0;
}

static void doClient(int clientSockfd, const char* clientIP, int clientPort,
    int serverRtpSockfd, int serverRtcpSockfd)
{
    char method[40];
    char url[100];
    char version[40];
    int cseq;
    int clientRtpPort{ -1 }, clientRtcpPort{-1};
    char* bufPtr;
    char* rBuf = (char*)malloc(BUF_MAX_SIZE);
    char* sBuf = (char*)malloc(BUF_MAX_SIZE);
    char line[400];

    while (1)
    {
        int recvLen;

        recvLen = recv(clientSockfd, rBuf, BUF_MAX_SIZE, 0);
        if (recvLen <= 0)
            goto out;

        rBuf[recvLen] = '\0';
        printf("---------------C->S--------------\n");
        printf("%s", rBuf);

        /* ???????? */
        bufPtr = getLineFromBuf(rBuf, line);
        if (sscanf(line, "%s %s %s\r\n", method, url, version) != 3)
        {
            printf("parse err\n");
            goto out;
        }

        /* ???????к? */
        bufPtr = getLineFromBuf(bufPtr, line);
        if (sscanf(line, "CSeq: %d\r\n", &cseq) != 1)
        {
            printf("parse err\n");
            goto out;
        }

        /* ??????SETUP????ô???ٽ???client_port */
        if (!strcmp(method, "SETUP"))
        {
            while (1)
            {
                bufPtr = getLineFromBuf(bufPtr, line);
                if (!strncmp(line, "Transport:", strlen("Transport:")))
                {
                    sscanf(line, "Transport: RTP/AVP;unicast;client_port=%d-%d\r\n",
                        &clientRtpPort, &clientRtcpPort);
                    break;
                }
            }
        }

        if (!strcmp(method, "OPTIONS"))
        {
            if (handleCmd_OPTIONS(sBuf, cseq))
            {
                printf("failed to handle options\n");
                goto out;
            }
        }
        else if (!strcmp(method, "DESCRIBE"))
        {
            if (handleCmd_DESCRIBE(sBuf, cseq, url))
            {
                printf("failed to handle describe\n");
                goto out;
            }
        }
        else if (!strcmp(method, "SETUP"))
        {
            if (handleCmd_SETUP(sBuf, cseq, clientRtpPort))
            {
                printf("failed to handle setup\n");
                goto out;
            }
        }
        else if (!strcmp(method, "PLAY"))
        {
            if (handleCmd_PLAY(sBuf, cseq))
            {
                printf("failed to handle play\n");
                goto out;
            }
        }
        else
        {
            goto out;
        }

        printf("---------------S->C--------------\n");
        printf("%s", sBuf);
        send(clientSockfd, sBuf, strlen(sBuf), 0);
        //return;
        if (!strcmp(method, "PLAY"))
        {
            auto udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
            sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_port = htons(4002);
            addr.sin_addr.S_un.S_addr = INADDR_ANY;
            if (bind(udp_sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
                printf("TCP::bind() failed.\n");
                return;
            }
            int num = 0;
            char buf[1024] = { 0 };
            sockaddr_in client;
            socklen_t client_len = sizeof(client);
            //send 
            auto send_sock = socket(AF_INET, SOCK_DGRAM, 0);
            sockaddr_in sin;
            sin.sin_family = AF_INET;
            sin.sin_port = htons(clientRtpPort);
            sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
            int len = sizeof(sin);
            char* rBuf = (char*)malloc(1024 * 1024);
            while (1) {
                //printf("waiting...\n");
                auto _size = recvfrom(udp_sock, buf, 1024, 0, reinterpret_cast<sockaddr*>(&client), &client_len);
                if (_size < 0) {
                    perror("recvfrom");
                }
                else if (_size == 0) {
                    printf("client shutdown...\n");
                }
                else {
                    auto cout = sendto(send_sock, buf, _size, 0, (sockaddr*)&sin, len);
                    printf("get# %d\n", cout);
                    memset(buf, 0, 1024);
                }
            }
        }
    }
out:
    closesocket(clientSockfd);
    //close(clientSockfd);
    free(rBuf);
    free(sBuf);
}

int main(int argc, char* argv[])
{
    //??ʼ??WSA
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA wsaData;
    if (WSAStartup(sockVersion, &wsaData) != 0)
    {
        return -1;
    }
    int serverSockfd;
    int serverRtpSockfd, serverRtcpSockfd;
    int ret;

    serverSockfd = createTcpSocket();
    if (serverSockfd < 0)
    {
        printf("failed to create tcp socket\n");
        return -1;
    }

    ret = bindSocketAddr(serverSockfd, "0.0.0.0", SERVER_PORT);
    if (ret < 0)
    {
        printf("failed to bind addr\n");
        return -1;
    }

    ret = listen(serverSockfd, 10);
    if (ret < 0)
    {
        printf("failed to listen\n");
        return -1;
    }

    serverRtpSockfd = createUdpSocket();
    serverRtcpSockfd = createUdpSocket();
    if (serverRtpSockfd < 0 || serverRtcpSockfd < 0)
    {
        printf("failed to create udp socket\n");
        return -1;
    }

    if (bindSocketAddr(serverRtpSockfd, "0.0.0.0", SERVER_RTP_PORT) < 0 ||
        bindSocketAddr(serverRtcpSockfd, "0.0.0.0", SERVER_RTCP_PORT) < 0)
    {
        printf("failed to bind addr\n");
        return -1;
    }

    printf("rtsp://127.0.0.1:%d\n", SERVER_PORT);

    while (1)
    {
        int clientSockfd;
        char clientIp[40];
        int clientPort;

        clientSockfd = acceptClient(serverSockfd, clientIp, &clientPort);
        if (clientSockfd < 0)
        {
            printf("failed to accept client\n");
            return -1;
        }

        printf("accept client;client ip:%s,client port:%d\n", clientIp, clientPort);

        doClient(clientSockfd, clientIp, clientPort, serverRtpSockfd, serverRtcpSockfd);
    }
    WSACleanup();
    return 0;

}
