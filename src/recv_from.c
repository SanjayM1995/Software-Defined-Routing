#include <sys/select.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/connection_manager.h"
#include "../include/control_header_lib.h"
#include "../include/init_response.h"

extern void print_ip(int);

void recv_from()
{
  printf("RECEIVING ROUTER PACKET\n");
  struct UPDATE_VECTOR *temp2;
  struct UPDATE_VECTOR temp_1;
  temp2 = (struct UPDATE_VECTOR*)malloc(sizeof(struct UPDATE_VECTOR)*48);
  struct sockaddr_in addr;
  bzero(&addr,sizeof(addr));
  socklen_t addr_len = sizeof(addr);
  if((recvfrom(router_socket,temp2,68,0,(struct sockaddr*)&addr,&addr_len))==-1)
  {
    perror("Error in receiving UDP\n");
  }
  else
  {
    // printf("######################################\n");
    // printf("NO OF UPDATE FIELDS : %i \n",(temp2->update_fields));
    // printf("SOURCE ROUTER : %i\n",(temp2->port));
    // // print_ip(ntohl(temp2->ip));
    // printf("%i",temp2->ip);
    // printf("\n");
    // for(int i=0;i<ntohs(temp2->update_fields);i++)
    // {
    //   print_ip(ntohl(temp2->temp[i].ip));
    //   printf("  %i ",ntohs(temp2->temp[i].port));
    //   printf("  %i",ntohs(temp2->temp[i].padding));
    //   printf("  %i",ntohs(temp2->temp[i].id));
    //   printf("  %i\n",ntohs(temp2->temp[i].cost));
    //   // printf("%i  ",(temp2->temp[i].ip));
    //   // printf("  %i ",(temp2->temp[i].port));
    //   // printf("  %i",(temp2->temp[i].padding));
    //   // printf("  %i",(temp2->temp[i].id));
    //   // printf("  %i\n",(temp2->temp[i].cost));
    // }
  }
  // Bellman Ford
  int check_id,replace_id;
  printf("MAIN ROUTER : %i\n",router_id_main);
  for(int i=0;i<ntohs(temp2->update_fields);i++)
  {
    if(ntohs(temp2->port) == ntohs(temp1.router[i].port))
    {
      check_id = i;
      replace_id = router[i].id;
      printf("ROUTER_ID : %i\n",router[i].id);
    }
  }
  printf("CHECK ID : %i\n",check_id);
  for(int i=0;i<ntohs(temp2->update_fields);i++)
  {
    if(ntohs(temp2->temp[i].cost)!=0 && ntohs(temp2->temp[i].cost)!=65535)
    {
      if((ntohs(temp2->temp[i].cost) + ntohs(route[check_id].old_cost)) < ntohs(route[i].cost))
      {
          route[i].cost = htons(ntohs(temp2->temp[i].cost) + ntohs(route[check_id].old_cost));
          route[i].id = htons((replace_id));
      }
    }
    else if(ntohs(temp2->temp[i].cost)!=65535)
    {
      if((ntohs(temp2->temp[i].cost) + ntohs(route[check_id].old_cost))<65535)
      {
        route[i].cost = htons(ntohs(temp2->temp[i].cost) + ntohs(route[check_id].old_cost));
        route[i].id = htons((replace_id));
      }
    }
  }
}
