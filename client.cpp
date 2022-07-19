#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define PORT 3490
#define SERVER_IP "127.0.0.1"

int main(){
    char command[1024];
    char ans[1024];
    int bytes_sent;
    struct sockaddr_in serverAddress;
    int sock = socket(AF_INET, SOCK_STREAM, 0); //open socket
    if (sock == -1) {
        perror("socket");
        return -1;
    }
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    int rval = inet_pton(AF_INET, (const char*)SERVER_IP, &serverAddress.sin_addr);//convert to binary 
    if (rval <= 0)
    {
        printf("inet_pton() failed");
        return -1;
    }
    if(connect(sock, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1){ //connect to server
            printf("connect() failed with error code : %d",errno);
        return 0;
    }
    while(1){
        printf("please write your command, if you want to exit write 'EXIT' and then 'enter'\n");
        memset(command, 0, 1024);
        memset(ans, 0, 1024);
        fgets(command, 1024, stdin);//get command for the keyboard
        bytes_sent = send(sock, command, sizeof(command), 0);//send command to the server
        if(bytes_sent == -1){
            perror("send failed\n");
        }
        if(strcmp(command, "EXIT\n") == 0){
            break;
        }
        recv(sock, ans, sizeof(ans), 0);//recieve answer from the server
        printf("OUTPUT: %s\n", ans);
    }
    close(sock);
    return 0;
}
