#include <string.h>
#include <inttypes.h>
#include <netinet/in.h>

#include "../include/global.h"
#include "../include/control_header_lib.h"
#include "../include/network_util.h"
#include "../include/init_response.h"

extern void print_ip(int);

void send_update(int sock_index,int flag)
{
  if(flag==0)
  {
    extern int neighbours[5];
    // for(int i=0;i<5;i++)
    // {
    //   printf("%s ",neighbours[i]?"true":"false");
    // }
    extern int router_socket;
    extern uint16_t  no_of_routers;
    for(int i=0;i<5;i++)
    {
      if(neighbours[i])
      {
        printf("Sending Message to : %i\n",i+1);
        struct sockaddr_in sock_addr;
        sock_addr.sin_port = htons(temp1.router[i].port);
        sock_addr.sin_addr.s_addr = temp1.router[i].ip;
        socklen_t addr_len;
        addr_len = sizeof(sock_addr);
        printf("SIZE OF TEMP : %i \n",sizeof(temp1));
        int size_temp;
        struct UPDATE_VECTOR upd;
        size_temp = sizeof(upd);
        upd.update_fields = htons(no_of_routers);
        for(int i=0;i<no_of_routers;i++)
        {
          if(route[i].cost == 0)
          {
            upd.port = temp1.router[i].port;
            upd.ip = temp1.router[i].ip;
          }
          upd.temp[i].ip = temp1.router[i].ip;
          upd.temp[i].port = temp1.router[i].port;
          upd.temp[i].padding = route[i].padding;
          upd.temp[i].id = route[i].id;
          upd.temp[i].cost = route[i].cost;
        }
        printf("BEFORE SENDING ###########################\n");
        printf("No of Update Fields : %i \n",upd.update_fields);
        printf("Source Port is :  %i",upd.port);
        printf("IP is : %i\n",upd.ip);
          for(int i=0;i<no_of_routers;i++)
          {
            print_ip(ntohl(upd.temp[i].ip));
            printf("  %i  ",ntohs(upd.temp[i].port));
            printf("  %i",ntohs(upd.temp[i].padding));
            printf("  %i",ntohs(upd.temp[i].id));
            printf("  %i\n",ntohs(upd.temp[i].cost));
            // printf("%i  ",upd.temp[i].ip);
            // printf("%i  ",(upd.temp[i].port));
            // printf("  %i",(upd.temp[i].padding));
            // printf("  %i",(upd.temp[i].id));
            // printf("  %i\n",(upd.temp[i].cost));

          }
        sendUDP(router_socket,&upd,size_temp,&sock_addr,addr_len);
      }
    }
  }
  /* SENDING THE PAYLOAD FOR ROUTING TABLE*/
  if(flag == 1)
  {
    printf("SENDING ROUTING TABLE :%i \n",sizeof(temp1));
    uint16_t payload_len,response_len;
    char *cntrl_response_header,*cntrl_response_payload,*cntrl_response;
    struct RESPONSE_PAYLOAD router[5];
    int r_id;
    for(int i=0;i<no_of_routers;i++)
    {
      printf("%i",ntohs(route[i].id1));
      printf("  %i",ntohs(route[i].padding));
      printf("  %i",ntohs(route[i].id));
      printf("  %i\n",ntohs(route[i].cost));
    }
    payload_len = no_of_routers*8;
    char* buffer = (char*) malloc(payload_len);
    memset(buffer,'\0',payload_len);
    int temp_len = 0;
    for(int i=0;i<no_of_routers;i++)
    {
      memcpy(buffer+temp_len,&(route[i].id1),2);
      memcpy(buffer+temp_len+2,&(route[i].padding),2);
      memcpy(buffer+temp_len+4,&(route[i].id),2);
      memcpy(buffer+temp_len+6,&(route[i].cost),2);
      temp_len = temp_len  + 8 ;
     printf("temp_len : %i\n",temp_len);
    }
    printf("BUFFER LENGTH : %i\n",strlen(buffer));
    printf("PAYLOAD_LEN : %i\n",payload_len);
    cntrl_response_payload = (char*)malloc(sizeof(char)*payload_len);
    memcpy(cntrl_response_payload,buffer,payload_len);
    cntrl_response_header = create_response_header(sock_index,2,0,payload_len);
    response_len = CNTRL_RESP_HEADER_SIZE + payload_len;
    cntrl_response = (char*)malloc(response_len);
    memcpy(cntrl_response,cntrl_response_header,CNTRL_RESP_HEADER_SIZE);
    free(cntrl_response_header);
    memcpy(cntrl_response+CNTRL_RESP_HEADER_SIZE,cntrl_response_payload,payload_len);
    free(cntrl_response_payload);
    sendALL(sock_index,cntrl_response,response_len);
    free(cntrl_response);
  }
}
