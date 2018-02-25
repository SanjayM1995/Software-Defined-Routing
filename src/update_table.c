#include <stdio.h>
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

void update_table(int sock_index,char* cntrl_payload)
{
  struct __attribute__ ((__packed__)) UPDATE
  {
    uint16_t r_id;
    uint16_t cost;
  };
  struct UPDATE *var = (struct UPDATE *) cntrl_payload;
  int old_value;
  old_value = route[(ntohs(var->r_id)-1)].cost;
  route[(ntohs(var->r_id)-1)].old_cost = var->cost;
  // if(
  //   route[(ntohs(var->r_id)-1)].id = var->r_id;
  //Controller Response
  uint16_t payload_len,response_len;
  char *cntrl_response_header,*cntrl_response_payload,*cntrl_response;
  payload_len = 0;
  cntrl_response_payload  = (char *)malloc(payload_len);
  memcpy(cntrl_response_payload,NULL,0);
  cntrl_response_header = create_response_header(sock_index,3,0,payload_len);
  response_len = CNTRL_RESP_HEADER_SIZE + payload_len;
  cntrl_response = (char*) malloc(response_len);
  memcpy(cntrl_response,cntrl_response_header,CNTRL_RESP_HEADER_SIZE);
  free(cntrl_response_header);
  memcpy(cntrl_response+CNTRL_RESP_HEADER_SIZE,cntrl_response_payload,payload_len);
  free(cntrl_response_payload);
  sendALL(sock_index,cntrl_response,response_len);
  free(cntrl_response);

}
