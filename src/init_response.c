#include <string.h>
#include <inttypes.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "../include/global.h"
#include "../include/control_header_lib.h"
#include "../include/network_util.h"
#include "../include/send_update.h"
#include "../include/connection_manager.h"
#include "../include/init_response.h"

#define PAY "SUCCESS"
/*https://stackoverflow.com/questions/1680365/integer-to-ip-address-c*/
void print_ip(int ip)
{
    unsigned char bytes[4];
    bytes[0] = ip & 0xFF;
    bytes[1] = (ip >> 8) & 0xFF;
    bytes[2] = (ip >> 16) & 0xFF;
    bytes[3] = (ip >> 24) & 0xFF;
    printf("%d.%d.%d.%d", bytes[3], bytes[2], bytes[1], bytes[0]);
}
void init_response(int sock_index,char* cntrl_payload)
{
  uint16_t payload_len,response_len;
  char *cntrl_response_header,*cntrl_response_payload,*cntrl_response;
  payload_len = 0;
  cntrl_response_payload  = (char *)malloc(payload_len);
  memcpy(cntrl_response_payload,NULL,0);
  cntrl_response_header = create_response_header(sock_index,1,0,payload_len);
  response_len = CNTRL_RESP_HEADER_SIZE + payload_len;
  cntrl_response = (char*) malloc(response_len);
  memcpy(cntrl_response,cntrl_response_header,CNTRL_RESP_HEADER_SIZE);
  free(cntrl_response_header);
  memcpy(cntrl_response+CNTRL_RESP_HEADER_SIZE,cntrl_response_payload,payload_len);
  free(cntrl_response_payload);
  sendALL(sock_index,cntrl_response,response_len);
  free(cntrl_response);
    extern int data_send_flag;
    data_send_flag = 0;
    extern fd_set master_list;
    extern int update_interval;
    extern uint16_t  no_of_routers;
    // struct __attribute__((__packed__)) router
    // {
    //   uint16_t id;
    //   uint16_t port;
    //   uint16_t data_port;
    //   uint16_t cost;
    //   uint32_t ip;
    // }router[5];
    struct CONTROL_PAYLOAD_INIT *header = (struct CONTROL_PAYLOAD_INIT*) cntrl_payload;
    no_of_routers = ntohs(header->no_of_routers);
    update_interval = ntohs(header->update_interval);
    printf("%i\n",update_interval);
    extern struct timeval tv;
    tv.tv_sec = update_interval;
    extern int router_socket,head_fd;
    for(int i=0;i<no_of_routers;i++)
    {
      if(header->router[i].cost==0)
      {
        extern int router_id_main;
        router_id_main = i+1;
        if((router_socket = socket(AF_INET,SOCK_DGRAM,0))==-1)
        {
          printf("Error creating Datagram port\n");
        }
        if(setsockopt(router_socket,SOL_SOCKET,SO_REUSEADDR,&(int){1},sizeof(int))<0)
          printf("Error in resuse address");
        if(setsockopt(router_socket,SOL_SOCKET,SO_REUSEPORT,&(int){1},sizeof(int))<0)
          printf("Error in reuse port");
        struct sockaddr_in addr;
        addr.sin_port = header->router[i].port;
        addr.sin_addr.s_addr = INADDR_ANY;
        if((bind(router_socket,(struct sockaddr*)&addr,sizeof(addr)))==-1)
        {
          printf("Unable to bind datagram socket\n");
        }
        else
        {
          FD_SET(router_socket,&master_list);
          if(router_socket>head_fd)
          head_fd = router_socket;
          printf("Datagram socket bound and initialized\n");
        }
        // neighbours[i] == NULL;
        extern int data_socket;
        data_socket = socket(PF_INET,SOCK_STREAM,0);
        if(setsockopt(data_socket,SOL_SOCKET,SO_REUSEADDR,&(int){1},sizeof(int))<0)
          printf("Error in resuse address");
        if(setsockopt(data_socket,SOL_SOCKET,SO_REUSEPORT,&(int){1},sizeof(int))<0)
          printf("Error in reuse port");
        struct sockaddr_in data_addr;
        data_addr.sin_family = PF_INET;
        data_addr.sin_port = header->router[i].data_port;
        printf("DATA INITIALIZE PORT : %i\n",ntohs(header->router[i].data_port));
        data_addr.sin_addr.s_addr = INADDR_ANY;
        if(bind(data_socket,(struct sockaddr*)&data_addr,sizeof(data_addr))<0)
          printf("Data Bind Failed");
        else
        {
          printf("Data Socket Bound : %i\n",data_socket);
          FD_SET(data_socket,&master_list);
          if(data_socket>head_fd)
          head_fd = data_socket;
        }
        if(listen(data_socket,5)<0)
          printf("Listen Failed");
        else
          printf("Data Socket Listening \n");
       }
      router[i].id = ntohs(header->router[i].id);
      temp1.router[i].id = header->router[i].id;
      router[i].port = ntohs(header->router[i].port);
      temp1.router[i].padding = htons(0);
      temp1.router[i].port = header->router[i].port;
      router[i].data_port = ntohs(header->router[i].data_port);
      router[i].cost = ntohs(header->router[i].cost);
      if(router[i].cost != 0 && router[i].cost!=65535)
      {
        neighbours[i] = 1;
      }
      temp1.router[i].cost = header->router[i].cost;
      router[i].ip = ntohl(header->router[i].ip);
      temp1.router[i].ip = header->router[i].ip;
    }
    for(int i=0;i<no_of_routers;i++)
    {
      route[i].id1 = router[i].id;
      route[i].padding = temp1.router[i].padding;
      if((ntohs(temp1.router[i].cost))!=65535)
      {
        route[i].id = router[i].id;
        route[i].cost = temp1.router[i].cost;
        route[i].old_cost = temp1.router[i].cost;
      }
      else
      {
        route[i].id = htons(65535);
        route[i].cost = htons(65535);
        route[i].old_cost = htons(65535);
      }
    }
   printf("Printing: \n");
   printf(" %i ",no_of_routers);
   printf(" %i \n",update_interval);
   printf("ID   PORT  DATA  COST      IP\n");
    for(int i=0;i<no_of_routers;i++)
    {
      printf("%i",router[i].id);
      printf("%8i",router[i].port);
      printf("%6i",router[i].data_port);
      printf("%6i     ",router[i].cost);
      print_ip(router[i].ip);
      printf("\n");
    }

    //send_update(1);
    //Response to the controller

}
