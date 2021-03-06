/*********************************************************
* Module Name: UDP Echo client source 
*
* File Name:    UDPEchoClient2.c
*
* Summary:
*  This file contains the echo Client code.
*
* Revisions:
*
* $A0: 6-15-2008: misc. improvements
*
*********************************************************/
#include "UDPEcho.h"
#include <netdb.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>

void clientCNTCCode();
void CatchAlarm(int ignored);

int numberOfTimeOuts=0;
unsigned int numberOfTrials;
unsigned long totalBytesSent;
unsigned long totalBytesLost;
unsigned long avgSendingRate;
double avgLossRate;
double totalRTT , minRTT = -1.0, maxRTT= -1.0 ;
int bStop;
struct timeval *opTime1;
int completeIter = 0;
int sock;                          /* Socket descriptor */
unsigned int bMode;
double* RTT;

char Version[] = "1.1";   

int main(int argc, char *argv[])
{
    struct sockaddr_in echoServAddr;    /* Echo server address */
    struct sockaddr_in fromAddr;     /* Source address of echo */
    unsigned short echoServPort;        /* Echo server port */
    unsigned int fromSize;           /* In-out of address size for recvfrom() */
    char *servIP;                       /* IP address of server */
    void* sndMessageData;                  /* String to send to echo server */
    void* recvMessageData;                  /* String to send to echo server */
    int respStringLen;               /* Length of received response */
    struct hostent *thehost;	        /* Hostent from gethostbyname() */

    //Consider time 1 for operation time, 2 for bucket time, 3 for RTT time
    struct timeval *opTime2;
    struct timeval *bucketTime1, *bucketTime2;
    struct timeval *rttTime1, *rttTime2;

    struct timeval opTV1, opTV2, bktTV1, bktTV2, rttTV1, rrtTV2;
    struct sigaction myaction;
    
    // 1 is for operation time, 2 for bucketTime, 3 for rtt time
    double usec1 = 0, usec2, usec3 = 0;
    int *seqNumberPtr;
    unsigned int seqNumber = 1;
    
    opTime1 = &opTV1;
    opTime2 = &opTV2;
    bucketTime1 = &bktTV1;
    bucketTime2 = &bktTV2;
    rttTime1 = &rttTV1;
    rttTime2 = &rrtTV2;

    //Initialize values
    numberOfTimeOuts = 0;
    totalBytesSent = 0;
    avgSendingRate = 0;
    avgLossRate = 0;
    totalRTT = 0;
    totalBytesLost = 0;
    numberOfTrials = 0;
    bStop = 0;
    
    //By default avg rate is 100000 bps
    unsigned long avgRate = 1000000;
    
    //By default bucket size is 1*token Size 
    unsigned int maxBucketSz = 1;
    unsigned int currBucketSz = 0;
    
    //By default tokenSize and message size is 1472 bytes
    unsigned int tokenSize = 1472;
    unsigned int messageSize = 1472;
    
    //Default mode is RTT i.e. 0 (For one way its 1)
    bMode = 0;
    
    //Default no. of iteration is 1
    int nIterations = 0;
    
    //Default value for debug flag is false
    int bDebugFlag = 0;
    
    double meanRTT = 0.0, stdRTT = 0.0;

    int i = 0;

    //Initialize values
    numberOfTimeOuts = 0;
    numberOfTrials = 0;
    bStop = 0;

    if (argc < 3)    /* Test for correct number of arguments */
    {
        fprintf(stderr,"Usage: %s <Server IP> [<Server Port>] \n", argv[0]);
        exit(1);
    }

    signal (SIGINT, clientCNTCCode);

    servIP = argv[1];                   /* First arg: server IP address (dotted quad) */
    echoServPort = atoi(argv[2]);       

    /* get info from parameters , or default to defaults if they're not specified */
    if (argc == 4) {
       avgRate = atoi(argv[3]);
    }
    else if (argc == 5) {
       avgRate = atoi(argv[3]);
       maxBucketSz = atoi(argv[4]);
    }
    else if (argc == 6) {
       avgRate = atoi(argv[3]);
       maxBucketSz = atoi(argv[4]);
       tokenSize = atoi(argv[5]);
    }
    else if (argc == 7) {
       avgRate = atoi(argv[3]);
       maxBucketSz = atoi(argv[4]);
       tokenSize = atoi(argv[5]);
       messageSize = atoi(argv[6]);
    }
    else if (argc == 8) {
       avgRate = atoi(argv[3]);
       maxBucketSz = atoi(argv[4]);
       tokenSize = atoi(argv[5]);
       messageSize = atoi(argv[6]);
       bMode = atoi(argv[7]);
       if(bMode > 1)
           bMode = 1;
    }
    else if (argc == 9) {
       avgRate = atoi(argv[3]);
       maxBucketSz = atoi(argv[4]);
       tokenSize = atoi(argv[5]);
       messageSize = atoi(argv[6]);
       bMode = atoi(argv[7]);
       if(bMode > 1)
           bMode = 1;
       nIterations = atoi(argv[8]);
    }
    else if (argc == 10) {
       avgRate = atoi(argv[3]);
       maxBucketSz = atoi(argv[4]);
       tokenSize = atoi(argv[5]);
       messageSize = atoi(argv[6]);
       bMode = atoi(argv[7]);
       if(bMode > 1)
           bMode = 1;
       nIterations = atoi(argv[8]);
       bDebugFlag = atoi(argv[9]);
    }
    
    //Convert avg rate to bytes from bits
    avgRate = avgRate/8;
    //Max bucket size will now contains values in bytes and not in tokens
    maxBucketSz = maxBucketSz * tokenSize;
       
    myaction.sa_handler = CatchAlarm;
    if (sigfillset(&myaction.sa_mask) < 0)
       DieWithError("sigfillset() failed");

    myaction.sa_flags = 0;

    if (sigaction(SIGALRM, &myaction, 0) < 0)
       DieWithError("sigaction failed for sigalarm");

    recvMessageData = malloc(messageSize);
    
    sndMessageData = malloc(messageSize);
    ((SndMsg*)sndMessageData)->MessageSize = htons(messageSize);
    ((SndMsg*)sndMessageData)->SequenceNumber = htonl(seqNumber++); ;
    ((SndMsg*)sndMessageData)->SessionMode = htons(bMode);
    
    RTT = calloc(nIterations, sizeof(double));
    double lastBktTime = 0.0;
    double currBktTime = 0.0;

    /* Construct the server address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));    /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                 /* Internet addr family */
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);  /* Server IP address */
    
    /* If user gave a dotted decimal address, we need to resolve it  */
    if (echoServAddr.sin_addr.s_addr == -1) {
        thehost = gethostbyname(servIP);
	    echoServAddr.sin_addr.s_addr = *((unsigned long *) thehost->h_addr_list[0]);
    }
    
    echoServAddr.sin_port   = htons(echoServPort);     /* Server port */

    gettimeofday(opTime1, NULL);
    gettimeofday(bucketTime1, NULL);
    lastBktTime = (bucketTime1->tv_sec) * 1000000 + (bucketTime1->tv_usec);
    
        /* Create a datagram/UDP socket */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");
    
    int bForEver = nIterations == 0 ? 1 : 0;
    while ((bForEver || nIterations > 0) && bStop != 1) 
    {
        if(nIterations == 1)
        {
            memset((sndMessageData+16), 1, 8);
        }
        
        if(messageSize > currBucketSz)
        {
            gettimeofday(bucketTime2, NULL);
            currBktTime = (bucketTime2->tv_sec) * 1000000 + (bucketTime2->tv_usec);
            usec2 = currBktTime - lastBktTime;
            lastBktTime = currBktTime;
            currBucketSz += avgRate * usec2;
            printf("Ohh bucket is leaky \n");
        }
        else
        {
            gettimeofday(rttTime1, NULL);

            if (sendto(sock, sndMessageData, messageSize, 0, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) != messageSize)
            {
              DieWithError("sendto() sent a different number of bytes than expected"); 
            }
            currBucketSz -= messageSize;
                       
            /* Recv a response */
            fromSize = sizeof(fromAddr);
            alarm(2);                       //set the timeout for 2 seconds

            if(bMode)
            {
                totalBytesSent +=  messageSize;
            }
            else
            {
                if ((respStringLen = recvfrom(sock, recvMessageData, messageSize, 0, (struct sockaddr *) &fromAddr, &fromSize)) != messageSize) 
                {
                    if (errno == EINTR) 
                    { 
                       printf("Received a  Timeout !!!!!\n"); 
                       numberOfTimeOuts++;
                       totalBytesLost += messageSize;
                    }
                }
                else
                {
                    alarm(0);                                   //clear the timeout 
                    totalBytesSent +=  messageSize;
                    gettimeofday(rttTime2, NULL);
                    usec3 = ((rttTime2->tv_sec) * 1000000 + (rttTime2->tv_usec)) - ((rttTime1->tv_sec) * 1000000 + (rttTime1->tv_usec));
                    printf("RTT value for complete iter %d is %f \n", completeIter, usec3);
                    
                    if(minRTT < 0)
                        minRTT = usec3;    
                    if(usec3 < minRTT)
		        minRTT = usec3;
                    if(usec3 > maxRTT)
		        maxRTT = usec3;
                    totalRTT += usec3;
                    RTT[completeIter++] = usec3;    
                }
            }
            
            nIterations--;
            numberOfTrials++; 
        }
    }
    
    free(recvMessageData);
    free(sndMessageData);
    close(sock);

    gettimeofday(opTime2, NULL);
    usec1 = ((opTime2->tv_sec) * 1000000 + (opTime2->tv_usec)) - ((opTime1->tv_sec) * 1000000 + (opTime1->tv_usec));

    if (numberOfTrials != 0) 
        avgSendingRate = (totalBytesSent * 1000000/usec1) * 8;
    
    if (numberOfTimeOuts != 0) 
        avgLossRate = ((numberOfTimeOuts*100)/numberOfTrials);
    
    meanRTT = (totalRTT/completeIter);
    meanRTT /= 1000000;                          /* Convert to seconds */
    
    //Standard Deviation
    for(i=0; i < completeIter; i++)
        stdRTT += (RTT[i]-meanRTT) * (RTT[i]-meanRTT);

    stdRTT /= numberOfTrials;
    stdRTT /= 1000000;                          /*Convert to seconds*/

    printf("\nTotal Msg Sent: %lu Avg sending rate: %lu seconds  ", totalBytesSent, avgSendingRate);
    if(bMode == 0)
    {
        printf("MinRTT: %0.6f MaxRTT: %0.6f MeanRTT: %0.6f StdDevRTT: %0.6f Avg loss rate: %0.3f \n", minRTT/1000000, maxRTT/1000000, meanRTT, stdRTT, avgLossRate);
    }
    exit(0);
}

void CatchAlarm(int ignored) { }

void clientCNTCCode() 
{
    double meanRTT = 0.0, stdRTT = 0.0;
    struct timeval *opTime;
    struct timeval opTV;
    double usec4 = 0.0;
    int i = 0;
    opTime = &opTV;
    gettimeofday(opTime, NULL);
    usec4 = ((opTime->tv_sec) * 1000000 + (opTime->tv_usec)) - ((opTime1->tv_sec) * 1000000 + (opTime1->tv_usec));

    avgSendingRate = 0;
    avgLossRate = 0;
    bStop = 1;
    
    if (numberOfTrials != 0) 
        avgSendingRate = (totalBytesSent * 1000000/usec4) * 8;
    
    if (numberOfTimeOuts != 0) 
        avgLossRate = ((numberOfTimeOuts*100)/numberOfTrials);
    
    meanRTT = (totalRTT/completeIter);
    
    //Standard Deviation
    for(i=0; i < completeIter; i++)
        stdRTT += (RTT[i]-meanRTT) * (RTT[i]-meanRTT);

    stdRTT /= numberOfTrials;
    
    printf("\nTotal Msg Sent: %lu Avg sending rate: %lu seconds  ", totalBytesSent, avgSendingRate);
    if(bMode == 0)
    {
        printf("MinRTT: %0.6f MaxRTT: %0.6f MeanRTT: %0.6f StdDevRTT: %0.6f Avg loss rate: %0.3f \n", minRTT/1000000, maxRTT/1000000, meanRTT, stdRTT, avgLossRate);
    }
    close(sock);
    exit(0);
}  
