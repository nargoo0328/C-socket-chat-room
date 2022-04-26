#pragma comment(lib, "libwsock32.a")
 
#include <WinSock2.h>
#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
//#include <unistd.h>
//using namespace std;
struct user{
	int is_set;
	char nickname[100];
	bool first;
};
user client_socket[10];
int client_num=0;
SOCKET socket_tcp;
int max(int a,int b){
	
	return ((a>=b)? a:b);
}
DWORD WINAPI WorkerThread(LPVOID lpParam) {
	int forClientSockfd;
    int maxfdp=socket_tcp;
    fd_set rset;
    FD_ZERO(&rset);
    char client_message[100];
    char message[200];
    SOCKADDR_IN clinetAddr;
    int addrlen = sizeof(clinetAddr);
    while(true){
    	FD_ZERO(&rset);
    	FD_SET(socket_tcp,&rset);
    	for(int i=0;i<client_num;i++){
    		if(client_socket[i].is_set==-1)
    			continue;
    		maxfdp=max(maxfdp,client_socket[i].is_set);
    		FD_SET(client_socket[i].is_set,&rset);
		}
		select(maxfdp+1, &rset, 0, 0, NULL);
		//connection
		if(FD_ISSET(socket_tcp,&rset))
        {
        	forClientSockfd = accept(socket_tcp, (SOCKADDR*)&clinetAddr, &addrlen);
        	client_socket[client_num].first = true;
        	client_socket[client_num++].is_set=forClientSockfd;
            printf("a connection was found.\n");
            printf("server: got connection from %s.\n", inet_ntoa(clinetAddr.sin_addr)); 
        }
        //Read from client
		for(int i=0;i<client_num;i++){
        	if(FD_ISSET(client_socket[i].is_set,&rset)){
				recv(client_socket[i].is_set,client_message, sizeof(client_message), 0);
				if(client_socket[i].first){
					printf("setting nickname as %s.\n",client_message);
            		strncpy(client_socket[i].nickname,client_message,strlen(client_message));
            		client_socket[i].first=false;
				}else{
					printf("%s: %s\n",client_socket[i].nickname,client_message);
					if(strncmp("Bye",client_message,sizeof("Bye"))==0){
						closesocket(client_socket[i].is_set);
						client_socket[i].is_set=-1;
						ZeroMemory(client_socket[i].nickname,100);
					}else{
						strncpy(message,client_socket[i].nickname,strlen(client_socket[i].nickname));
						strcat(message," : ");
						strcat(message,client_message);
						for(int j=0;j<client_num;j++){
							if (j==i)
								continue;
							send(client_socket[j].is_set,message,sizeof(message),0);
						}
					}
					ZeroMemory(message,200);
				}
				ZeroMemory(client_message, 100);
			}
		}
		
	}
}
int main(int argc, char *argv[])
{
	char temp_message[100];
    int r;
    WSAData wsaData;
    WORD DLLVSERION;
    DLLVSERION = MAKEWORD(2,1);
 	DWORD       dwThreadId;
    r = WSAStartup(DLLVSERION, &wsaData);
    SOCKADDR_IN addr;
    int addrlen = sizeof(addr);
    socket_tcp = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[2]));
    if(bind(socket_tcp, (SOCKADDR*)&addr, sizeof(addr))){
		printf("Bind failed!\n");
		return 0;
	}
    listen(socket_tcp, 10);
	printf("Server is online.\n");
    SOCKADDR_IN clinetAddr;
    int forClientSockfd;
    int maxfdp=socket_tcp;
    CreateThread(NULL, 0, WorkerThread, NULL, 0, &dwThreadId);
    while(true)
    {
    	gets(temp_message);
    	for(int i=0;i<client_num;i++)
    		send(client_socket[i].is_set,temp_message,sizeof(temp_message),0);
    }
    return 0;
}
