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
   int listenfd = 0,connfd = 0, natfd=0, byte_size;
   struct sockaddr_in serv_addr; 
   struct sockaddr_in nat_addr;

   float time[1024];
   int vmn[1024];
   int prt[1024];
   char sendBuff[1025];  
   FILE *fp;
   
   listenfd = socket(AF_INET, SOCK_STREAM, 0);
   if((natfd = socket(AF_INET, SOCK_STREAM, 0))< 0)
   {
      printf("\n Error : Could not create socket \n");
      return 1;
       }
   //printf("socket retrieve success\n");
  
   memset(&serv_addr, '0', sizeof(serv_addr));
   memset(&nat_addr, '0', sizeof(nat_addr));
   memset(sendBuff, '0', sizeof(sendBuff));

   //memset(time, '0', sizeof(time));
   //memset(vmn, '0', sizeof(vmn));
   //memset(prt, '0', sizeof(prt));
   bzero(time, 1024);
   bzero(vmn, 1);
   bzero(prt, 1);
   
   
   //printf("recieved data: %f\n", time);
   
   nat_addr.sin_family = AF_INET;    
   nat_addr.sin_addr.s_addr = inet_addr("192.168.0.1"); 
   nat_addr.sin_port = htons(10001);       

   serv_addr.sin_family = AF_INET;    
   serv_addr.sin_addr.s_addr = inet_addr("192.168.0.2"); 
   serv_addr.sin_port = htons(10002);    
 
   bind(listenfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr));
  
   if(listen(listenfd, 10) == -1){
      printf("Failed to listen\n");
      return -1;
   }

   // accept awaiting request
   connfd = accept(listenfd, (struct sockaddr*)NULL ,NULL); 

   if(connect(natfd, (struct sockaddr *)&nat_addr, sizeof(nat_addr))
     <0)
   {
      printf("\n Error : NAT Connect Failed \n");
      return 1;
       }
      
      
      /*byte_size = recv(connfd,time,1024,0);
      time[byte_size] = '\0';
      printf("recieved data: %f\n", time);
      
      strcpy(sendBuff, "Acknowledgements time");
      send(connfd, sendBuff,strlen(sendBuff), 0);
      */
      
      //VMN
      
   fp = fopen("connect.out","w");   
      
   strcpy(sendBuff, "Acknowledgements\n");
   while((vmn[0]!= -1)&&(prt[0]!=-1)){
      byte_size = recv(connfd,vmn,4,0);
      vmn[byte_size] = '\0';
      if(vmn[0] != -1){
         printf("recieved data: %i\n", vmn[0]);
         fprintf(fp, "%i,",vmn[0]);
      }
      send(natfd, sendBuff,strlen(sendBuff), 0);
         
      byte_size = recv(connfd,prt,4,0);
      vmn[byte_size] = '\0';
      if(vmn[0] != -1){
         printf("recieved data: %i\n", prt[0]);   
         fprintf(fp, "%i\n",prt[0]);
      }
      send(natfd, sendBuff,strlen(sendBuff), 0);
   }
   
   fclose(fp);
   
   close(listenfd);
   close(natfd);       
   close(connfd);    
    
   return 0;
}
