#include <winsock2.h>
#include <Windows.h>
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

//////////////////////////////////////////////////////////////////  CLIENT
	
	SOCKADDR_IN sin;// = {AF_INET};
	SOCKET m_ClientSock = socket(AF_INET, SOCK_STREAM, 0);

    //String sIP = ui->lineEdit_IP->text();
    //std::string str = sIP.toStdString();
    //char * pStr = (char *)str.c_str();   

    char * pStr = new char[50];
	cout<<"IP-address> ";
    cin.getline(pStr, 50); // "127.0.0.1"
    
    char sNull[sizeof(SOCKADDR_IN)] = {0};
    memcpy(&sin, sNull, sizeof(SOCKADDR_IN));
    sin.sin_family = AF_INET;
    sin.sin_port = htons((unsigned short)PORT);

    sin.sin_addr.S_un.S_addr = inet_addr(pStr);


	if(::connect(m_ClientSock, (sockaddr*)&sin, sizeof(sin)) == SOCKET_ERROR)
    {
        cout<< "ERROR " << "Cannot connect socket"<<endl;
        return 0;
    }
	//SOCKET m_WorkSock = accept(m_AcceptSock,(sockaddr*)&m_sin, NULL);
	do{
	send(m_ClientSock, pStr, 51, 0);
		
	recv(m_ClientSock, pStr, 51, 0);
	cout<<"Server:$ ";
	cout<<pStr<<endl;
	cout<<"Client:$ :";
	cin.getline(pStr, 50);
	} while(memcmp(pStr, "exit", 4));
	closesocket(m_ClientSock);
	WSACleanup();
return 0;
}