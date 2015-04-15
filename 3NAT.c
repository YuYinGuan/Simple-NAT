#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
 
int main(void)
{
   int clientfd = 0,client_connfd = 0;
   int serverfd = 0;
   int natfd = 0, nat_connfd = 0;
   
   struct sockaddr_in client_addr;
   struct sockaddr_in server_addr;
   struct sockaddr_in nat_addr;

   char ACK[1024];  
   int Data[1024];  
   
   clientfd = socket(AF_INET, SOCK_STREAM, 0);
   serverfd = socket(AF_INET, SOCK_STREAM, 0);
   natfd = socket(AF_INET, SOCK_STREAM, 0);
   
   //printf("socket retrieve success\n");
  
   memset(&client_addr, '0', sizeof(client_addr));
   memset(&server_addr, '0', sizeof(server_addr));
   memset(&nat_addr, '0', sizeof(nat_addr));
   
   bzero(ACK, 1024);
   bzero(Data, 1024);
   
   client_addr.sin_family = AF_INET;    
   client_addr.sin_addr.s_addr = inet_addr("10.0.0.2"); 
   client_addr.sin_port = htons(10000);    
 
   server_addr.sin_family = AF_INET;    
   server_addr.sin_addr.s_addr = inet_addr("192.168.0.2"); 
   server_addr.sin_port = htons(10002);  

   nat_addr.sin_family = AF_INET;    
   nat_addr.sin_addr.s_addr = inet_addr("192.168.0.1"); 
   nat_addr.sin_port = htons(10001);      
 
   bind(clientfd, (struct sockaddr*)&client_addr,sizeof(client_addr));
   bind(natfd, (struct sockaddr*)&nat_addr,sizeof(nat_addr));
  
   if(listen(clientfd, 10) == -1){
      printf("Failed to listen1\n");
      return -1;
   }
   
   client_connfd = accept(clientfd, (struct sockaddr*)NULL ,NULL);
   
   if(connect(serverfd, (struct sockaddr *)&server_addr, 
                                  sizeof(server_addr))<0)
   {
      printf("\n Error : Connect Failed \n");
      return 1;
       }

   if(listen(natfd, 10) == -1){
      printf("Failed to listen2\n");
      return -1;
   }

   nat_connfd = accept(natfd, (struct sockaddr*)NULL ,NULL);

   while(1){
      recv(client_connfd,Data,4,0);
      send(serverfd, Data, sizeof(int), 0);
      recv(nat_connfd,ACK,1024,0);
      send(client_connfd,ACK,strlen(ACK), 0);
   }
   return 0;
}
   
   
   
   
