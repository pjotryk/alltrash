#include <iostream>
#include <cstdio>
#include <Winsock2.h>
#include <Windows.h>
#include <windows.h>
#include <ws2tcpip.h>


using namespace std;

bool Initialize ();

int main(int argc, char* argv[])
{
	if (Initialize () == false)
	{
		cerr<<"false"<<endl;
		return -1;
	}
	
	char pszRemoteHost[255] = "google.com";
	char *pszIPAddress;
	
	hostent *pHostent = gethostbyname (pszRemoteHost);
	if (pHostent == NULL)
	{
		cerr << endl << "An error occured in gethostbyname operation: WSAGetLastError () = " << WSAGetLastError () << endl;
		return false;
	}

	in_addr in;
	memcpy(&in,  pHostent->h_addr_list [0], sizeof (in_addr));
	
	cout<< (int)in.S_un.S_un_b.s_b1 <<'.';
	cout<< (int)in.S_un.S_un_b.s_b2 <<'.';
	cout<< (int)in.S_un.S_un_b.s_b3 <<'.';
	cout<< (int)in.S_un.S_un_b.s_b4 <<endl;
	
	system("pause");
	
	
	
	pszIPAddress = inet_ntoa (in);		//problem
	
	cout<< "inet_ntoa" << endl; 
	system("pause");
	
	cout<< pszIPAddress<<endl;

	return true;
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