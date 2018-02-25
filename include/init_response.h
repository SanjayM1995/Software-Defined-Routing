#ifndef IN_RESPONSE_H_
#define IN_RESPONSE_H_

void init_response(int sock_index,char* cntrl_payload);
uint16_t  no_of_routers;
int update_interval;
int neighbours[5];
int router_id_main;
int data_send_flag;
#endif
