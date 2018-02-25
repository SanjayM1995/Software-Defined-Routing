#include <stdio.h>
#include <sys/select.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "../include/recv_file.h"
#include "../include/control_header_lib.h"
#include "../include/init_response.h"
#include "../include/network_util.h"

extern void print_ip(int);

void recv_file(int sock_index,char *payload)
{
    // printf("In RECV FILES\n");
    extern uint16_t no_of_routers;
    struct DATA_PACKET *d_packet;
    extern struct ROUTER_DATA router[5];
    char* data_pay = (char*)malloc(1024);
    strcpy(data_pay,payload+12);
    // printf("DATA : %s\n",data_pay);
    d_packet = (struct DATA_PACKET*)payload;
    extern uint16_t no_of_routers;
    int flag;
    flag = 0;
     printf("TTL : %u\n",d_packet->ttl);
     printf("Transfer ID : %u\n",d_packet->tid);
     printf("Seq Number : %i\n",ntohs(d_packet->seq_num));
     printf("PRINT IP : %i\n",d_packet->ip);
    for(int i=0;i<no_of_routers;i++)
    {
      // printf("%i\n",ntohl(temp1.router[i].ip));
      if(d_packet->ip == temp1.router[i].ip && temp1.router[i].cost==0)
      {

        flag=1;
      }
    }
    if(flag==1)
    {
      // printf("TTL : %u\n",d_packet->ttl);
      // printf("Transfer ID : %u\n",d_packet->tid);
      // printf("Seq Number : %i\n",ntohs(d_packet->seq_num));
      // printf("RECEIVING\n");
      char* data_pay = (char*)malloc(1024);
      strcpy(data_pay,payload+12);
      char* f_name;
      f_name = (char*)malloc(12);
      strcpy(f_name,"file-");
      char *t_id;
      t_id = (char*)malloc(5);
      sprintf(t_id,"%u",d_packet->tid);
      strcat(f_name,t_id);
      FILE* fp;
      fp = fopen(f_name,"a");
      fwrite(data_pay,1,1024,fp);
      fclose(fp);
    }
    else
    {
      d_packet->ttl = d_packet->ttl-1;
      if(d_packet->ttl!=0)
      {
        int id;
        // printf("IP : %i\n",d_packet->ip);
        for(int i=0;i<no_of_routers;i++)
        {
          printf("%i\n",ntohl(router[i].ip));
          if(d_packet->ip == ntohl(router[i].ip))
          {
              id =ntohs(route[i].id);
              // printf("%i %i %i ",route[i].id,route[i].id1,route[i].cost);
          }
        }
        // printf("Rerouting to id : %i\n",id);
        // printf("Rerouting to port : %i\n",(router[id-1].data_port));
        struct DATA_PACKET *temp_data;
        temp_data = malloc(12);
        temp_data->ttl = d_packet->ttl;
        temp_data->tid = d_packet->tid;
        temp_data->seq_num = d_packet->seq_num;
        temp_data->padding = d_packet->padding;
        temp_data->ip = d_packet->ip;
        // printf("IP : %i",temp_data->ip);

        char* data_reroute_response;
        data_reroute_response = (char*) malloc(1036);
        memcpy(data_reroute_response,temp_data,12);
        memcpy(data_reroute_response+12,data_pay,1024);
        int sock;
        sock = socket(AF_INET,SOCK_STREAM,0);
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(router[id-1].data_port);
          // printf("DATA PORT : %i\n",router[id-1].data_port);
        addr.sin_addr.s_addr = d_packet->ip;
        // print_ip(ntohl(d_packet->ip));
        //   printf("\n");
        if(setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&(int){1},sizeof(int))<0)
          printf("Error in reuse address");
        if(setsockopt(sock,SOL_SOCKET,SO_REUSEPORT,&(int){1},sizeof(int))<0)
          printf("Error in reuse port");
        if(sock<0)
          printf("Cannot Create A Socket\n");
        if(connect(sock,(struct sockaddr*)&addr,sizeof(addr))<0)
          perror("Connect Failed");
        else
          printf("Sucessful Connect");
        // printf("Rerouting\n");
        ssize_t len;
        len = 1036;
        //sendALL(sock,data_reroute_response,len);
        ssize_t bytes = send(sock,data_reroute_response,1036,0);
        if(bytes == 0)
          {printf("Not Sending\n");}
          while(bytes != 1036)
              bytes += send(sock,data_reroute_response+bytes, 1036-bytes, 0);
        free(data_reroute_response);
        close(sock);
      }
    }
    // printf("Data is : %s\n",data_pay);
    // printf("FILE NAME : %s",file_name);

}
