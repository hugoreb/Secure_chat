#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#define PORT 18909

void replace_n(char str[30]);

char room_name[30] = "Secure_chatroom";

void *getMsg(void *socket_thread)
{
    int socket = *(int*)socket_thread;
    char msgr[30];
    char last_msgr[30] = "";
    char username[30];
    char room_recv[30];
    int stay = 1;
    while(stay){
        recv(socket, &room_recv, sizeof(room_recv), 0);
        recv(socket, &username, sizeof(username), 0);
        recv(socket, &msgr, sizeof(msgr),0);
        if(strcmp(msgr,"") && strcmp(msgr, last_msgr)){
            if(strcmp(room_name,room_recv) == 0){
                replace_n(username);
                replace_n(msgr);
                printf("%s : %s\n",username,msgr);
                strcpy(last_msgr,msgr);
            }
        }   
    }
    close(socket);
    free(socket_thread);
    pthread_exit(NULL);

}
    

int main(){
    struct sockaddr_in addrServer;
    int socketServ = socket(AF_INET, SOCK_STREAM, 0); // AF_INET = IPv4 SOCK_STREAM = TCP
    pthread_t thread_recv_msg;   
    addrServer.sin_addr.s_addr = inet_addr("127.0.0.1"); // we define the IP as the loopback IP 
    addrServer.sin_family = AF_INET; // we're using IPv4
    addrServer.sin_port = htons(PORT); // used port

    bind(socketServ, (const struct sockaddr *)&addrServer, sizeof(addrServer));

    listen(socketServ, 5); // on écoute les connections entrantes
    
    struct sockaddr_in addrClient;
    socklen_t csize = sizeof(addrClient);
    pthread_t threadClient;
    int socketClient = accept(socketServ, (struct sockaddr *)&addrClient, &csize);
    printf("New connection\n\n");

    int *sendMsg = malloc(sizeof(int));
    *sendMsg = socketClient;

    pthread_create(&thread_recv_msg, NULL, getMsg, sendMsg);

    char msgs[50];
    char username[30] = "Server";
    char password[30];
    char last_msg[50] = "";
    char chatroom[30] = "Secure_chatroom";

    while(1){
        printf("Send your message\n");
        fgets(msgs, sizeof(msgs), stdin);
        while(!strcmp(msgs,last_msg)) {
            printf("Don't spam please.\n");
            fgets(msgs,sizeof(msgs),stdin);
        }
        if(strcmp(msgs,"\0") && strcmp(msgs, "\n") && strcmp(msgs, "\n\0")) {
            send(socketClient, &chatroom, sizeof(chatroom), 0);
            send(socketClient, &username, sizeof(username), 0);
            send(socketClient, &msgs, sizeof(msgs), 0);
            printf("Message sent\n");
        }
    }


}

void replace_n(char str[30]){
    str[strcspn(str, "\n")] = 0;
} 