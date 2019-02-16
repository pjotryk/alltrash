// Ping.cpp : Defines the entry point for the console application.
//g++ Ping.cpp -o Ping.exe -lws2_32

#include <iostream>
#include <cstdio>
#include <winsock2.h>
#include <Windows.h>
#include <windows.h>
#include <ws2tcpip.h>


struct ICMPheader
{
	unsigned char	byType;
	unsigned char	byCode;
	unsigned short	nChecksum;
	unsigned short	nId;
	unsigned short	nSequence;
};

struct IPheader
{
    unsigned char	byVerLen;        
    unsigned char	byTos;       
    unsigned short	nTotalLength;   
    unsigned short	nId;            
    unsigned short	nOffset;        
    unsigned char	byTtl;          
    unsigned char	byProtocol;     
    unsigned short	nChecksum;      
    unsigned int	nSrcAddr;       
    unsigned int	nDestAddr;      
};

using namespace std;

unsigned short CalcChecksum (char *pBuffer, int nLen);
bool ValidateChecksum (char *pBuffer, int nLen);
bool Initialize ();
bool UnInitialize ();
bool ResolveIP (char *pszRemoteHost, char *pszIPAddress);
void PrintUsage ();

int main(int argc, char* argv[])
{
	if (argc < 2 || argc > 5)
	{
		PrintUsage ();
		return 0;
	}

	if (Initialize () == false)
	{
		return -1;
	}

	int nSequence = 0;
	int nMessageSize = 32;	//The message size that the ICMP echo request should carry with it
	int nTimeOut = 5000;	//Request time out for echo request (in milliseconds)
	int nCount = 4;	//Number of times the request has to be send

	char *pszRemoteIP = NULL, *pSendBuffer = NULL, *pszRemoteHost = NULL;

	pszRemoteHost = argv [1];
	
	for (int i = 2; i < argc; ++i)
	{
		switch (i)
		{
		case 2:			
			nCount = atoi (argv [2]);
			break;
		case 3:
			nMessageSize = atoi (argv [3]);
			break;
		case 4:
			nTimeOut = atoi (argv [4]);
			break;
		}
	}

	if (ResolveIP (pszRemoteHost, pszRemoteIP) == false)
	{
		cerr << endl << "Unable to resolve hostname" << endl;
		return -1;
	}

	cout << "Pinging " << pszRemoteHost << " [" << pszRemoteIP << "] with " << nMessageSize << " bytes of data." << endl << endl;
	ICMPheader sendHdr;

	SOCKET sock;
	sock = socket (AF_INET, SOCK_RAW, IPPROTO_ICMP) ;	//Create a raw socket which will use ICMP
		if(sock == INVALID_SOCKET)
		cout<<"Socket not create    WSAGetLastError() - " << WSAGetLastError()<<endl;
	cout<<INVALID_SOCKET<<endl;
	cout<<sock<<endl;

	SOCKADDR_IN dest;	//Dest address to send the ICMP request
	dest.sin_addr.S_un.S_addr = inet_addr (pszRemoteIP);
	dest.sin_family = AF_INET;
	dest.sin_port = rand ();	//Pick a random port

	int nResult = 0;

	fd_set fdRead;
	SYSTEMTIME timeSend, timeRecv;
	int nTotalRoundTripTime = 0, nMaxRoundTripTime = 0, nMinRoundTripTime = -1, nRoundTripTime = 0;
	int nPacketsSent = 0, nPacketsReceived = 0;

	timeval timeInterval = {0, 0};
	timeInterval.tv_usec = nTimeOut * 1000;
	
	sendHdr.nId = htons (rand ());	//Set the transaction Id

	while (nPacketsSent < nCount)
	{
		//Створіть буфер повідомлень, який є достатньо великим для зберігання заголовка та даних повідомлення
		pSendBuffer = new char [sizeof (ICMPheader) + nMessageSize];
				
		sendHdr.byCode = 0;	//Нуль для ICMP відлуння та відповіді
		sendHdr.nSequence = htons (nSequence++);
		sendHdr.byType = 8;	//Вісім для ICMP ехо повідомлення
		sendHdr.nChecksum = 0;	//Контрольна сума розраховується пізніше
		
		memset (pSendBuffer + sizeof (ICMPheader), 'x', nMessageSize);	//Заповніть повідомлення довільним значенням
		
		//Обчислити контрольну суму за заголовком ICMP та даними повідомлень
		sendHdr.nChecksum = htons (CalcChecksum (pSendBuffer, sizeof (ICMPheader) + nMessageSize));	

		//Скопіюйте заголовок повідомлення назад у буфер
		memcpy(pSendBuffer,  &sendHdr, sizeof (ICMPheader));
		cout<<endl;

		nResult = sendto (sock, pSendBuffer, sizeof (ICMPheader) + nMessageSize, 0, (SOCKADDR *)&dest, sizeof (SOCKADDR_IN));
		cout << " nResult - " << nResult<<endl;
		
		//Збережіть час, коли було надіслано повідомлення ехоподібного протоколу ICMP
		::GetSystemTime (&timeSend);

		++nPacketsSent;

		if (nResult == SOCKET_ERROR)
		{
			cerr << endl << "An error occured in sendto operation: " << "WSAGetLastError () = " << WSAGetLastError () << endl;
			UnInitialize ();
			delete []pSendBuffer;
			return -1;
		}
		
		FD_ZERO (&fdRead);
		FD_SET (sock, &fdRead);

		if ((nResult = select (0, &fdRead, NULL, NULL, &timeInterval))
			== SOCKET_ERROR)
		{
			cerr << endl << "An error occured in select operation: " << "WSAGetLastError () = " << 
				WSAGetLastError () << endl;
			delete []pSendBuffer;
			return -1;
		}

		if (nResult > 0 && FD_ISSET (sock, &fdRead))
		{
			//Виділити великий буфер для зберігання відповіді
			char *pRecvBuffer = new char [1500];

			if ((nResult = recvfrom (sock, pRecvBuffer, 1500, 0, 0, 0)) 
				== SOCKET_ERROR)
			{
				cerr << endl << "An error occured in recvfrom operation: " << "WSAGetLastError () = " << 
					WSAGetLastError () << endl;
				UnInitialize ();
				delete []pSendBuffer;
				delete []pRecvBuffer;
				return -1;
			}
			
			//Get the time at which response is received
			::GetSystemTime (&timeRecv);

			//We got a response so we construct the ICMP header and message out of it
			ICMPheader recvHdr;
			char *pICMPbuffer = NULL;

			//The response includes the IP header as well, so we move 20 bytes ahead to read the ICMP header
			pICMPbuffer = pRecvBuffer + sizeof(IPheader);

			//ICMP message length is calculated by subtracting the IP header size from the 
			//total bytes received
			int nICMPMsgLen = nResult - sizeof(IPheader);

			//Construct the ICMP header
			memcpy(&recvHdr, pICMPbuffer, sizeof (recvHdr));

			//Construct the IP header from the response
			IPheader ipHdr;
			memcpy(&ipHdr, pRecvBuffer, sizeof (ipHdr));

			recvHdr.nId = recvHdr.nId;
			recvHdr.nSequence = recvHdr.nSequence;
			recvHdr.nChecksum = ntohs (recvHdr.nChecksum);

			//Check if the response is an echo reply, transaction ID and sequence number are same
			//as for the request, and that the checksum is correct
			if (recvHdr.byType == 0 &&
				recvHdr.nId == sendHdr.nId &&
				recvHdr.nSequence == sendHdr.nSequence &&
				ValidateChecksum (pICMPbuffer, nICMPMsgLen)  && 
				memcmp (pSendBuffer + sizeof(ICMPheader), pRecvBuffer + sizeof (ICMPheader) + sizeof(IPheader), 
					nResult - sizeof (ICMPheader) - sizeof(IPheader)) == 0)
			{
				//All's OK
				int nSec = timeRecv.wSecond - timeSend.wSecond;
				if (nSec < 0)
				{
					nSec = nSec + 60;
				}

				int nMilliSec = abs (timeRecv.wMilliseconds - timeSend.wMilliseconds);

				int nRoundTripTime = 0;
				nRoundTripTime = abs (nSec * 1000 - nMilliSec);

				cout << "Reply from " << pszRemoteIP << ": bytes = " << nResult - sizeof (ICMPheader) - sizeof (IPheader) 
					<< ", time = " << nRoundTripTime << "ms, TTL = " << (int)ipHdr.byTtl << endl;

				nTotalRoundTripTime = nTotalRoundTripTime + nRoundTripTime;

				if (nMinRoundTripTime == -1)
				{
					nMinRoundTripTime = nRoundTripTime;
					nMaxRoundTripTime = nRoundTripTime;
				}
				else if (nRoundTripTime < nMinRoundTripTime)
				{
					nMinRoundTripTime = nRoundTripTime;
				}
				else if (nRoundTripTime > nMaxRoundTripTime)
				{
					nMaxRoundTripTime = nRoundTripTime;
				}

				++nPacketsReceived;
			}
			else
			{
				cout << "The echo reply is not correct!" << endl;
			}

			delete []pRecvBuffer;
		}
		else
		{
			cout << "Request timed out." << endl;
		}

		delete []pSendBuffer;
	}

	cout << endl << "Ping statistics for " << pszRemoteIP << ":" << endl << '\t' << "Packets: Sent = " << nPacketsSent << ", Received = " << 
		nPacketsReceived << ", Lost = " << (nPacketsSent - nPacketsReceived) << " (" << 
		((nPacketsSent - nPacketsReceived)/(float)nPacketsSent) * 100 << "% loss)" << endl << '\t';

	if (nPacketsReceived > 0)
	{
		cout << "\rApproximate round trip times in milli-seconds:" << endl << '\t' << "Minimum = " << nMinRoundTripTime << 
			"ms, Maximum = " << nMaxRoundTripTime << "ms, Average = " << nTotalRoundTripTime / (float)nPacketsReceived << "ms" << endl;
	}

	cout << '\r' << endl;

	if (UnInitialize () == false)
	{
		return -1;
	}

	return 0;
}

unsigned short CalcChecksum (char *pBuffer, int nLen)
{
	//Checksum for ICMP is calculated in the same way as for
	//IP header

	//This code was taken from: http://www.netfor2.com/ipsum.htm

	unsigned short nWord;
	unsigned int nSum = 0;
	int i;
    
	//Make 16 bit words out of every two adjacent 8 bit words in the packet
	//and add them up
	for (i = 0; i < nLen; i = i + 2)
	{
		nWord =((pBuffer [i] << 8)& 0xFF00) + (pBuffer [i + 1] & 0xFF);
		nSum = nSum + (unsigned int)nWord;	
	}
	
	//Take only 16 bits out of the 32 bit sum and add up the carries
	while (nSum >> 16)
	{
		nSum = (nSum & 0xFFFF) + (nSum >> 16);
	}

	//One's complement the result
	nSum = ~nSum;
	
	return ((unsigned short) nSum);
}

bool ValidateChecksum (char *pBuffer, int nLen)
{
	unsigned short nWord;
	unsigned int nSum = 0;
	int i;
    
	//Make 16 bit words out of every two adjacent 8 bit words in the packet
	//and add them up
	for (i = 0; i < nLen; i = i + 2)
	{
		nWord =((pBuffer [i] << 8)& 0xFF00) + (pBuffer [i + 1] & 0xFF);
		nSum = nSum + (unsigned int)nWord;	
	}
	
	//Take only 16 bits out of the 32 bit sum and add up the carries
	while (nSum >> 16)
	{
		nSum = (nSum & 0xFFFF) + (nSum >> 16);
	}

	//To validate the checksum on the received message we don't complement the sum
	//of one's complement
	//One's complement the result
	//nSum = ~nSum;
	
	//The sum of one's complement should be 0xFFFF
	return ((unsigned short)nSum == 0xFFFF);
}

bool Initialize ()
{
	//Initialize WinSock
	WSADATA wsaData;

	if (WSAStartup (MAKEWORD (2, 2), &wsaData) == SOCKET_ERROR)
	{
		cerr << endl << "An error occured in WSAStartup operation: " << "WSAGetLastError () = " << WSAGetLastError () << endl;
		return false;
	}

	SYSTEMTIME time;
	::GetSystemTime (&time);

	//Seed the random number generator with current millisecond value
	srand (time.wMilliseconds);

	return true;
}

bool UnInitialize ()
{
	//Cleanup
	if (WSACleanup () == SOCKET_ERROR)
	{
		cerr << endl << "An error occured in WSACleanup operation: WSAGetLastError () = " << WSAGetLastError () << endl;
		return false;
	}

	return true;
}

bool ResolveIP (char *pszRemoteHost, char *pszIPAddress)
{
	hostent *pHostent = gethostbyname (pszRemoteHost);
	if (pHostent == NULL)
	{
		cerr << endl << "An error occured in gethostbyname operation: WSAGetLastError () = " << WSAGetLastError () << endl;
		return false;
	}

	in_addr in;
	memcpy(&in,  pHostent->h_addr_list [0], sizeof (in_addr));
	pszIPAddress = inet_ntoa (in);

	return true;
}

void PrintUsage ()
{
	cout << "Usage: ping r n b t" << endl << endl;
	cout << "  r - Remote host" << endl;
	cout << "  n - Number of echo requests to send" << endl;
	cout << "  b - Bytes to send" << endl;
	cout << "  t - Timeout after these many milliseconds" << endl << endl;

	cout << "\rping microsoft.com 4 32 4000" << endl << endl;
}