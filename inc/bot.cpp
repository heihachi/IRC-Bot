/*
    Main file for the bot
*/

#include "bot.h"
#define DEFAULT_BUFLEN 1024
#define MAXDATASIZE 1024

const bool debug = true;

IrcBot::IrcBot(Config config)
{
    hostName = config.server.c_str();
    portString = config.port.c_str();
    channelName = config.channel.c_str();
    userName = config.userName.c_str();
    nickName = config.nickName.c_str();
    userPass = config.userPass.c_str();
}

IrcBot::~IrcBot()
{
    #ifdef WIN32
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR)
    {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
    }
    #elif LINUX
    close(ConnectSocket);
    #endif // WIN32
}

bool IrcBot::bot()
{
    char *text;
    char buf[1024];
    int bytes_read;
    int bytes_send;

    string sendCommands[5] = { "PONG", "USER "+userName+" 8 * :"+userName, "NICK "+nickName, "PRIVMSG nickserv identify "+userPass, "JOIN "+channelName };

    printf("Connecting to %s:%s\nNick: %s | Channel: %s\n", hostName.c_str(), portString.c_str(), nickName.c_str(), channelName.c_str());

    ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    const char *sendbuf = "\nPONG :Bot\n";
    char recvbuf[DEFAULT_BUFLEN] = { };
    int recvbuflen = DEFAULT_BUFLEN;

    #ifdef WIN32
    // start WinSock Initialization
    WSADATA wsaData;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }
    #endif // WIN32

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(hostName.c_str(), portString.c_str(), &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        #ifdef WIN32
        WSACleanup();
        #endif // WIN32
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
        #ifdef WIN32
        WSACleanup();
        #endif // WIN32
        return 1;
    }

    // Send an initial buffer
    iResult = send( ConnectSocket, sendbuf, (int)strlen(sendbuf), 0 );
    if(DEBUG == true)
        printf("Sendbuf: %s\n", sendbuf);
    if (iResult == SOCKET_ERROR)
    {
        #ifdef WIN32
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        #endif // WIN32
        return 1;
    }

    printf("Bytes Sent: %ld\n", iResult);

    // Receive until the peer closes the connection
    int count = 0;
    while(1)
    {
        count++;

        switch(count)
        {
            case 2:
                // we send data to server after 3 counts
                // nick
                sendCommand(ConnectSocket, "\nUSER %s Bot Bot : %s\n", userName.c_str(), userName.c_str());
                // user
                sendCommand(ConnectSocket, "\nNICK %s\r\n", nickName.c_str());
                break;
            case 11:
                // register
                sendCommand(ConnectSocket, "\nPRIVMSG nickserv identify %s\r\n", userPass.c_str());
                break;
            case 12:
                // we join channel
                sendCommand(ConnectSocket, "\nJOIN %s\r\n", channelName.c_str());
            default:
                break;
        }

        // Recv and print data
        bytes_read = recv(ConnectSocket, buf, MAXDATASIZE-1, 0);
        buf[bytes_read]='\0';
        cout << buf << endl;
        // buf is the data that is recieved

        // pass buffer to message handler
        msgHandler(buf);

        // if we get ping
        if(charSearch(buf,"PING"))
        {
            sendPong(buf);
        }

        //break if connection closed
        if(bytes_read == 0)
        {
            cout << "Connection closed!" << endl;
            cout << timeNow() << endl;

            break;
        }
    }
#ifdef WIN32
    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();
#elif LINUX
    close(ConnectSocket);
#endif
    return true;
}

bool IrcBot::charSearch(char *toSearch, char *searchFor)
{
    int len = strlen(toSearch);
    int forLen = strlen(searchFor); // The length of the searchfor field

    //Search through each char in toSearch
    for (int i = 0; i < len;i++)
    {
        //If the active char is equil to the first search item then search toSearch
        if (searchFor[0] == toSearch[i])
        {
            bool found = true;
            //search the char array for search field
            for (int x = 1; x < forLen; x++)
            {
                if (toSearch[i+x]!=searchFor[x])
                {
                    found = false;
                }
            }

            //if found return true;
            if (found == true)
                return true;
        }
    }

    return 0;
}

bool IrcBot::isConnected(char *buf)
{
    //If we find /MOTD then its ok join a channel
    if (charSearch(buf,"/MOTD") == true)
        return true;
    else
        return false;
}

char * IrcBot::timeNow()
{
    time_t rawtime;
    struct tm * timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    return asctime(timeinfo);
}

// this will be used later with mysql
bool IrcBot::sendData(string msg)
{
    //send some data
    int len = strlen(msg.c_str());
    msg = msg+"/r/n";
    int btyes_sent = send(ConnectSocket, msg.c_str(), len, 0);
    cout << "Bytes Sent: " << btyes_sent << endl;
    if(btyes_sent == 0)
    {
        cout << "Couldn't send " << msg << endl;
        return false;
    }
    else
    {
        cout << "Sent " << msg << endl;
    }
}

int IrcBot::sendCommand(int sock, const char *format_p, ...)
{
    va_list args;
    char buffer[512] = {0};

    va_start(args, format_p);
    vsnprintf(buffer, sizeof(buffer), format_p, args);
    va_end(args);

    printf(">> %s>>\n", buffer);

    return (send(sock, buffer, strlen(buffer), 0));
}

bool IrcBot::sendPong(char *buf)
{
    // get reply address
    // loop through bug and find location of PING
    // search through each char in toSearch

    char * toSearch = "PING";

    for(int i = 0;i<strlen(buf);i++)
    {
        if(buf[i] == toSearch[0])
        {
            bool found = true;
            for(int x = 1;x<4;x++)
            {
                if(buf[i+x] != toSearch[x])
                    found = false;
            }
            // if found return true
            if(found == true)
            {
                int count = 0;
                //Count the chars
                for (int x = (i+strlen(toSearch)); x < strlen(buf);x++)
                {
                    count++;
                }

                //Create the new char array
                char returnHost[count + 5];
                returnHost[0]='P';
                returnHost[1]='O';
                returnHost[2]='N';
                returnHost[3]='G';
                returnHost[4]=' ';

                count = 0;
                //set the hostname data
                for (int x = (i+strlen(toSearch)); x < strlen(buf);x++)
                {
                    returnHost[count+5]=buf[x];
                    count++;
                }

                //send the pong
                //if (sendData(returnHost))
                if(sendCommand(ConnectSocket, "%s\r\n", returnHost))
                {
                    cout << "Sent ping pong ~~ " << timeNow() << endl;
                }
                return true;
            }
        }
    }
}

bool IrcBot::msgHandler(char * buf)
{
    // add code here to do a search
    if(charSearch(buf, "hello"))
        sendData("PRIVMSG "+channelName+" :hello\r\n");
    return true;
}
