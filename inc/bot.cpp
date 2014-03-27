/*
    Main file for the bot
*/
#include "config.h"
#include "bot.h"
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

bool debug = true;

bool bot(string config[8][2])
{
    string hostName = config[0][1].c_str();
    string portString = config[1][1].c_str();
    string channelName = config[2][1].c_str();
    string userName = config[3][1].c_str();
    string nickName = config[4][1].c_str();

    string sendCommands[3] = { "PONG attack", "USER "+userName, "NICK "+nickName };
    char *text;
    char buf[1024];
    int nbtyes_read;
    printf("Connecting to %s:%s\nNick: %s | Channel: %s\n", hostName.c_str(), portString.c_str(), nickName.c_str(), channelName.c_str());
    // This is to output the config
/*    for(int x = 0;x<8;x++)
        for(int y = 0;y<2;y++)
            printf("%-25s\n", config[x][y].c_str());
*/
    // start WinSock Initialization
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    const char *sendbuf = sendCommands[0].c_str();
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int nbytes_read;
    int recvbuflen = DEFAULT_BUFLEN;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(hostName.c_str(), portString.c_str(), &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR)
        {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    // Send an initial buffer
    iResult = send( ConnectSocket, sendbuf, (int)strlen(sendbuf), 0 );
    printf("Sendbuf: %s\n", sendbuf);
    if (iResult == SOCKET_ERROR)
    {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    printf("Bytes Sent: %ld\n", iResult);

    // shutdown the connection since no more data will be sent
//    iResult = shutdown(ConnectSocket, SD_SEND);
//    if (iResult == SOCKET_ERROR) {
//        printf("shutdown failed with error: %d\n", WSAGetLastError());
//        closesocket(ConnectSocket);
//        WSACleanup();
//        return 1;
//    }
    bool sentData = false;
    // Receive until the peer closes the connection
    do {

        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if ( iResult > 0 )
        {
            printf("Bytes received: %d\n%s\n", iResult, recvbuf);
            if(sentData == false)
            {
                sentData = true;
                for(int x = 0;x<3;x++)
                {
                    // need to set a recv() here
                    sendbuf = sendCommands[x].c_str();
                    printf("Sending: %s\n", sendbuf);
                    nbtyes_read = send( ConnectSocket, sendbuf, (int)strlen(sendbuf), 0 );
                    printf("Bytes received: %d\n%s\n", iResult, recvbuf);
                }
            }
        }
        else if ( iResult == 0 )
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());

    } while( iResult > 0 );

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return true;
}
