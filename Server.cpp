    #include <winsock2.h>
    #include <windows.h>
	#include <iostream>
	using namespace std;

#define PORT 5678

int main()
{
	WSADATA m_WSADATA;   

   if(WSAStartup(0x0202,&m_WSADATA))
    {
        cout<<"Canot init socket library"<<endl;
		WSACleanup();
		return 0;
    }
//////////////////////////////////////////////////////////////////////  SERVER
	SOCKADDR_IN m_sin;
	SOCKET m_AcceptSock = socket(AF_INET, SOCK_STREAM, 0);
	
	m_sin.sin_port = htons((unsigned short)PORT);
	m_sin.sin_family = AF_INET;
    m_sin.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (bind(m_AcceptSock, (sockaddr*)&m_sin, sizeof(m_sin)) == SOCKET_ERROR)
            cout << " Cannot bind socket"<< endl;

    if (listen(m_AcceptSock, SOMAXCONN) == SOCKET_ERROR)
            cout<<" Cannot enter listen mode"<< endl;

	SOCKET m_WorkSock = accept(m_AcceptSock,(sockaddr*)&m_sin, NULL);


    char sBuff[50];
	
	do{
	if(recv(m_WorkSock, sBuff, 50, 0))
		cout<<"Client:$ ";
	cout<<sBuff<<endl;
	cout<<"Server:$ ";
	cin.getline(sBuff, 50);
	send(m_WorkSock, sBuff, 50, 0);
	} while(sBuff!="exit");
	closesocket(m_WorkSock);

WSACleanup();
return 0;
}