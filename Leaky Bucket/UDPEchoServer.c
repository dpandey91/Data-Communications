/*********************************************************
*
* Module Name: UDP Echo server 
*
* File Name:    UDPEchoServer.c	
*
* Summary:
*  This file contains the echo server code
*
* Revisions:
*  $A0:  6/15/2008:  don't exit on error
*
*********************************************************/
#include "UDPEcho.h"

#ifdef _DEBUG
#define LOG_MSG(...) printf(__VA_ARGS__) // Or simply LOG_MSG(msg) printf(msg)
#else
#define LOG_MSG(...)                     // Or LOG_MSG(msg)
#endif

//Maximum message size is defined as 60K
#define MAX_MSG_SIZE 60000

//Define maximum client connections server can handle
#define MAX_CLIENT_CONN 100

typedef struct clientConnectInfo{
  struct in_addr client_ipAddr;
  unsigned int port;
  unsigned long clientStartTs;
  unsigned long clientEndTs;
  unsigned long nBytesRecvd;
  unsigned long nBytesLost;
  unsigned int noOfRecieves;
} connInfo;

char bStop = 0;
int totalClientConnections = 0;
connInfo* connectInfo = 0;
int sock;                           /* Socket */

void DieWithError(char *errorMessage);  /* External error handling function */

void serverCNTCCode();

//Returns 0 is false else true
int checkAllOne(unsigned int value)
{ 
    unsigned int mask = (1ULL << 32) - 1;
    value &= mask;
    return value == mask || value == 0;
}

double generateRandom()
{
    double num = (double)rand() / (double)RAND_MAX ;
    printf("Random number generated is %f", num);
    return num;
}

int main(int argc, char *argv[])
{
    struct sockaddr_in echoServAddr;    /* Local address */
    struct sockaddr_in echoClntAddr;    /* Client address */
    unsigned int cliAddrLen;            /* Length of incoming message */
    unsigned short echoServPort;        /* Server port */
    void* receiveBuffer;                /* Buffer for receive string */
    int recvMsgSize;                    /* Size of received message */
    void* sndBuffer;                    /* Buffer for send string */
    int sndMsgSize;                    /* Size of send message */
    double randomRate, lossRate;        /* Lost rate at which server should simulate packet loss */
    int debugFlag;                      /* Debug flag option */
    
    struct timeval *ptrTime;
    struct timeval time;
    ptrTime = &time;
    
    //Set default new connection to 1 i.e. its new
    int i = 0, bCheckNewConn = 1, bPacketLoss = 0;
    
    unsigned int seqNumber = 0;
    
    int recvPacketMsgSz = 0;
    
    //Mode of operation. Default is RTT i.e. 0
    int bMode = 0;
    
    if (argc < 2)                   /* Test for correct number of parameters */
    {
        fprintf(stderr,"Usage:  %s <UDP SERVER PORT>\n", argv[0]);
        exit(1);
    }

    echoServPort = atoi(argv[1]);   /* First arg:  local port */
    
    if(argc == 2)
    {
        lossRate = 0.0;            /* Signifies no loss rate */
        debugFlag = 0;
    }
    else if(argc == 3)
    {
        lossRate = atof(argv[2]);
        debugFlag = 0;
    }
    else if(argc >= 4)
    {
        lossRate = atof(argv[2]);
        debugFlag = atoi(argv[3]);
    }
    
    printf("UDPEchoServer Leaky Bucket implemenatation: Server starting at Port:%d\n", echoServPort);    

    receiveBuffer = malloc(MAX_MSG_SIZE);
        
    //Set signal handler so we can exit
    signal(SIGINT, serverCNTCCode);
    
    /* Create socket for sending/receiving datagrams */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0){
      printf("Failure on socket call , errno:%d\n",errno);
    }

    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Local port */

    //Allocate memory for storing client connections information
    //TODO: 100 to 70 to handle 70 clients connections
    connectInfo = calloc(MAX_CLIENT_CONN, sizeof(connInfo));
    
    /* Bind to the local address */
    if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0) {
          printf("Failure on bind, errno:%d\n",errno);
    }
    
    for (;;) /* Run forever */
    {
        /* Set the size of the in-out parameter */
        cliAddrLen = sizeof(echoClntAddr);

        /* Block until receive message from a client */
        if ((recvMsgSize = recvfrom(sock, receiveBuffer, MAX_MSG_SIZE, 0,
            (struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
        {
            printf("Failure on recvfrom, client: %s, errno:%d\n", inet_ntoa(echoClntAddr.sin_addr),errno);
        }
        else 
        {
            printf("Recvd from client %s\n", inet_ntoa(echoClntAddr.sin_addr));
            
            recvPacketMsgSz = ntohs(((RecvMsg*)receiveBuffer)->MessageSize);
            bMode = ntohs(((RecvMsg*)receiveBuffer)->SessionMode);
            seqNumber = ntohl(((RecvMsg*)receiveBuffer)->SequenceNumber);
                        
            randomRate = generateRandom();
            if(randomRate <= lossRate)
            {
                bPacketLoss = 1;
            }
            
            //Get time to save it for calculating elapsed time. 
            //Should update time with every receive for specific client
            gettimeofday(ptrTime, NULL);
            
            //Check if this recieve from is actually from existing client or a new client
            //Iterating through list of connections and checking if recv from client is present in it
            for(i = 0; i < totalClientConnections; i++)
            {
              //If we find a match, go ahead and add in the data
              if(connectInfo[i].client_ipAddr.s_addr == echoClntAddr.sin_addr.s_addr && connectInfo[i].port == echoClntAddr.sin_port)
              {
                  // Set new connection to 0 (false) if its present in connInfo array and stop iterating
                  // Also if its present update info accordingly
                  bCheckNewConn  = 0;
                  connectInfo[i].noOfRecieves++;
                  connectInfo[i].clientEndTs = (ptrTime->tv_sec) * 1000000 + (ptrTime->tv_usec);
                  
                  if(bPacketLoss){
                      connectInfo[i].nBytesLost += recvMsgSize;
                  }
                  else{
                      connectInfo[i].nBytesRecvd += recvMsgSize;
                  }
                  break;
              }
            }
                 
            if(bCheckNewConn)
            {
                printf("The client %s session is created", inet_ntoa(echoClntAddr.sin_addr));
                
                connectInfo[totalClientConnections].client_ipAddr = echoClntAddr.sin_addr;
                connectInfo[totalClientConnections].port = echoClntAddr.sin_port;
                connectInfo[totalClientConnections].noOfRecieves = 1;
                connectInfo[totalClientConnections].clientStartTs = (ptrTime->tv_sec) * 1000000 + (ptrTime->tv_usec);
                connectInfo[totalClientConnections].clientEndTs = connectInfo[totalClientConnections].clientStartTs;

                if(bPacketLoss){
                  connectInfo[i].nBytesLost = recvMsgSize;    
                }
                else{
                  connectInfo[i].nBytesRecvd = recvMsgSize;
                }

                totalClientConnections++;
            }
            
            //Reset so we can handle new connection next time
            bCheckNewConn = 1;
            
            if(bMode == 0 && bPacketLoss == 0)
            {
                //This means its RTT mode
                if(checkAllOne(seqNumber))
                {
                    if (sendto(sock, receiveBuffer, recvMsgSize, 0, (struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr)) != recvMsgSize) 
                    {
                        printf("Failure on sendTo, client: %s, errno:%d\n", inet_ntoa(echoClntAddr.sin_addr),errno);
                    }    
                }
                else
                {
                    printf("The client %s session is terminated", inet_ntoa(echoClntAddr.sin_addr));
                }
            }
            free(receiveBuffer);
        }        
    }
}

void serverCNTCCode()
{
    char clientIpAddr[INET_ADDRSTRLEN];
    int port = 0;
    float sessionDuration = 0.0;
    unsigned long totalClientBytesRecvd = 0, clientBytesRecvd = 0;
    unsigned long avgThroughput = 0;
    double avgLossRate = 0;

    int i = 0;

    for(i = 0; i < totalClientConnections; i++)
    {
        totalClientBytesRecvd += connectInfo[i].nBytesRecvd;
    }    
    
    printf("No. of connections: %d Total bytes recvd: %lu\n", totalClientConnections, totalClientBytesRecvd);

    for(i = 0; i < totalClientConnections; i++)
    {
        //The Ip address of client
        inet_ntop(AF_INET, &(connectInfo[i].client_ipAddr), clientIpAddr, INET_ADDRSTRLEN);
        port = connectInfo[i].port;
        
        //Session duration in sec, thus divided by 1000000
        sessionDuration = (float)(connectInfo[i].clientEndTs - connectInfo[i].clientStartTs) / (float) 1000000;

        //Bytes recvd in each session
        clientBytesRecvd = connectInfo[i].nBytesRecvd;

        //Avg throughput in bps thus * by 8
        avgThroughput = (unsigned long)(clientBytesRecvd / sessionDuration) * 8;
        
        //Avg loss rate 
        avgLossRate = (double) connectInfo[i].nBytesLost / sessionDuration;
        
        printf("For each client=> IP: %s Port: %d SessionDuration: %0.3f BytesReceived: %lu AvgThrouput: %lu AvgLossRate: %f \n", 
                clientIpAddr, port, sessionDuration, clientBytesRecvd, avgThroughput, avgLossRate);
    }
    
    close(sock);
    free(connectInfo);
    exit(0);
}
