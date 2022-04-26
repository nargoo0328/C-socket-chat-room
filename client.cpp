#pragma comment(lib, "libwsock32.a")
#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
SOCKET socket_tcp;
DWORD WINAPI WorkerThread(LPVOID lpParam) {
	char server_message[200];
	while(true){
		ZeroMemory(server_message, 100);
		recv(socket_tcp,server_message, sizeof(server_message), 0);
    	printf("%s\n",server_message);
	}
}
int main(int argc, char *argv[])
{
    char message[100];
    int r;
    WSAData wsaData;
    WORD DLLVersion;
    DLLVersion = MAKEWORD(2,1);
    r = WSAStartup(DLLVersion, &wsaData);
	DWORD       dwThreadId;
    SOCKADDR_IN addr;
 
    int addlen = sizeof(addr);
 
 
    //AF_INET: internet-family
    //SOCKET_STREAM: connection-oriented socket
    socket_tcp = socket(AF_INET, SOCK_STREAM, NULL);
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[2]));
    if(connect(socket_tcp, (SOCKADDR*)&addr, sizeof(addr))<0){
		printf("Connection failed!\n");
		return 0;
	}
    ZeroMemory(message, 100);
    CreateThread(NULL, 0, WorkerThread, NULL, 0, &dwThreadId);
    printf("Please enter your nickname.\n");
    gets(message);
    send(socket_tcp, message, (int)strlen(message), 0);
    while(true){
    	ZeroMemory(message, 100);
        gets(message);
        send(socket_tcp, message, (int)strlen(message), 0); 
		if(strncmp("Bye",message,sizeof("Bye"))==0)
			break;
	}
    closesocket(socket_tcp);
    return 0;
 
}
