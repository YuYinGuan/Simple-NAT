#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
 
 
#define MAX_LEN 1024
#define END_R -1 

int main(int argc,char *argv[])
{
   
   if(argc != 2){
      perror("check arguments");
   }
   
   float time[MAX_LEN];
   char line[MAX_LEN];
   int vmn[MAX_LEN];
   int prt[MAX_LEN];
   int line_data = 0;
   
   FILE *fp;
   FILE *temp;
   char* token;
 
   printf("Enter the name of file you wish to see\n");
   
   fp = fopen(argv[1],"r"); // read mode
   temp = fopen(argv[1],"r");
   if( fp == NULL )
   {
      perror("Error while opening the file.\n");
      exit(EXIT_FAILURE);
   }
 
   printf("The contents of %s file are :\n", argv[1]);
   
   
   while( fgets(line, MAX_LEN, fp) != NULL)  {
        token = strtok(line, ",\n");
        while( token!=NULL ){
            time[line_data] = atof(token);
            token = strtok(NULL, ",\n");
         vmn[line_data] = atoi(token);
            token = strtok(NULL, ",\n");
         prt[line_data] = atoi(token);
            token = strtok(NULL, ",\n");
         line_data++;
            }
   }
   fclose(fp);
   
   time[line_data] = END_R;
   vmn[line_data] = END_R;
   prt[line_data] = END_R;
   
   int cp = 0;
   while(cp<=line_data){
      int np =1;
      if(((time[cp+1] - time[cp])< 1) && (time[cp+1]!=END_R)){
         fprintf(stderr,"Error: Period between %f,%i,%i ",
                                  time[cp],vmn[cp],prt[cp]); 
         fprintf(stderr,"and %f,%i,%i is less than 1 second\n"
                           ,time[cp+1],vmn[cp+1],prt[cp+1]);
         while(time[cp+np]!=END_R){
            time[cp+np] = time[cp+np+1];
            vmn[cp+np] = vmn[cp+np+1];
            prt[cp+np] = prt[cp+np+1];
            np++;
         }
         line_data--;
         //printf("deleting x when cp is %i\n", cp);
      }else{
         cp++;
      }
   }
   
   cp = 0;
   
   int x,y,z;

   for(x=0;x<line_data;x++){
      for(y=1;y<(line_data -x);y++){
         if(((vmn[x] == vmn[x+y])&&(prt[x]!=prt[x+y])) || 
               ((prt[x] == prt[x+y])&&(vmn[x]!=vmn[x+y]))){
            fprintf(stderr,"Error: %f,%i,%i taken VM name or Port, ",
                                              time[x],vmn[x],prt[x]); 
            fprintf(stderr,"so %f,%i,%i is Invalid\n",
                             time[x+y],vmn[x+y],prt[x+y]);
            for(z=0;z<(line_data -x -y); z++){
               vmn[x+y+z]= vmn[x+y+z+1];
               prt[x+y+z]= prt[x+y+z+1];
               time[x+y+z]= time[x+y+z+1];               
            }
            //printf("Deleted here %i,%i\n",x,y);

            line_data--;
         }
      }
   }

   for(y=0; y<=line_data;y++){
   
      printf("%f %i %i %d \n", time[y], vmn[y], prt[y], line_data);
   }

   
   int sockfd = 0, byte_size;
   char recvBuff[1024];
   char sendBuff[1024];
   
   memset(recvBuff, '0' ,sizeof(recvBuff));
   memset(sendBuff, '0' ,sizeof(sendBuff));
  
   struct sockaddr_in serv_addr;
   
   if((sockfd = socket(AF_INET, SOCK_STREAM, 0))< 0)
   {
      printf("\n Error : Could not create socket \n");
      return 1;
    }
 
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_port = htons(10000);
   serv_addr.sin_addr.s_addr = inet_addr("10.0.0.2");
 
   if(connect(sockfd, (struct sockaddr *)&serv_addr, 
    sizeof(serv_addr))< 0)
   {
      printf("\n Error : Connect Failed \n");
      return 1;
       }
   
   //sleep(0.2);
   
   int sent_loc = 0;
   int temp_send[1];

   float time_now = 0;
   
   while(sent_loc<=line_data){
      if(time[sent_loc]!=-1){
         sleep(time[sent_loc] - time_now);
         time_now = time[sent_loc];
      }
      
      temp_send[0] = vmn[sent_loc];
      //printf("here %i\n",sent_loc);
      send(sockfd, temp_send, sizeof(int), 0);
      byte_size = recv(sockfd,recvBuff,1024,0);
      recvBuff[byte_size] = '\0';
      printf("%s\n", recvBuff);
      
      //printf("here %i\n",sent_loc);
      temp_send[0] = prt[sent_loc];
      send(sockfd, temp_send, sizeof(int), 0);
      byte_size = recv(sockfd,recvBuff,1024,0);
      recvBuff[byte_size] = '\0';
      printf("%s\n", recvBuff);
      sent_loc++;
   }
   close(sockfd);
   
   return 0;
}
