#include <stdio.h>
#include <sys/select.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/send_file.h"
#include "../include/init_response.h"
#include "../include/control_header_lib.h"
#include "../include/network_util.h"

extern void print_ip(int);
extern ssize_t sendALL(int, char*, ssize_t);

void send_file(int sock_index,int payload_len,char* cntrl_payload)
{
  int offset;
  offset = payload_len - 8;
  printf("Payload Len : %i\n",payload_len);
  printf("Offset : %i\n",offset);
  struct FILE_PAYLOAD *header = (struct FILE_PAYLOAD*) cntrl_payload;
  char* file_name = (char*)malloc(offset);
  strcpy(file_name,cntrl_payload+8);
  print_ip(ntohl(header->ip));
  printf("\n");
  printf("TTL : %i\n",header->ttl);
  printf("Transfer ID : %u\n",header->tid);
  printf("Seq Number : %u\n",ntohs(header->seq_num));
  printf("FILE NAME : %s",file_name);
  int dest;
  extern uint16_t no_of_routers;
  for(int i=0;i<no_of_routers;i++)
  {
    if(ntohl(header->ip) == ntohl(temp1.router[i].ip))
    {
      dest = ntohs(route[i].id);
      printf("Destination is : %i\n",dest);
    }
  }
  int sock;
  sock = socket(AF_INET,SOCK_STREAM,0);
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(router[dest-1].data_port);
  printf("DATA PORT : %i\n",router[dest-1].data_port);
  addr.sin_addr.s_addr = htonl(router[dest-1].ip);
  print_ip(router[dest-1].ip);
  printf("\n");
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
  char* tok;
  char* check_file;
  check_file = (char*)malloc(sizeof(file_name));
  strcpy(check_file,file_name);
  // tok = strtok(check_file,".");
  // tok = strtok(NULL,".");
  // if(!strcmp(tok,"bin"))
  // {
  //   printf("BINARY FILE\n");
  // }
  // else
  //   printf("TEXT FILE\n");
  // printf("FILENAME TOKENIZED: %s\n", tok);
  FILE *fp;
  fp = fopen(file_name,"rb");
  if(fp==NULL)
  {
    printf("Error opening file\n");
    fp = fopen(file_name,"rb");
  }
  if(fseek(fp,0,SEEK_END)!=0)
  {
    printf("SEEK ERROR\n");
  }
  long file_size;
  file_size = ftell(fp);
  if(file_size == -1)
  {
    printf("Cannot find file size");
  }
  printf("FILE SIZE IS  : %i\n",file_size);
  //Packing the structure with necessary details
  struct DATA_PACKET d_packet;
  d_packet.ip = header->ip;
  d_packet.tid = header->tid;
  d_packet.ttl = header->ttl;
  d_packet.seq_num = header->seq_num;
  if(file_size<1024)
  {
    d_packet.padding = 0;
  }
  else
  {
    d_packet.padding = 0;
  }
  fseek(fp,0,SEEK_SET);
  char* buffer = (char*)malloc(sizeof(char)*1024);
  fread(buffer,1,1024,fp);
//  printf("CONTENTS OF FILE : %s\n",buffer);
  char *data_header,*data_payload,*data_response;
  data_header = (char*)malloc(sizeof(d_packet)*sizeof(char));
  memcpy(data_header,&d_packet,sizeof(d_packet));
  data_response = (char*)malloc(sizeof(d_packet)+1024);
  memcpy(data_response,data_header,sizeof(data_header));
  data_payload = (char*)malloc(1024);
  memcpy(data_payload,buffer,1024);
  memcpy(data_response+12,data_payload,1024);
  sendALL(sock,data_response,1036);
  int n;
  n = file_size/1024;
  for(int i=1;i<n;i++)
  {
    if(i==n-1)
    {
      char* fin  = "10000000000000000000000000000000";
      d_packet.padding = htons(atoi(fin));
    }
    fseek(fp,1024*i,SEEK_SET);
    char* buffer = (char*)malloc(sizeof(char)*1024);
    fread(buffer,1,1024,fp);
    d_packet.seq_num = d_packet.seq_num+htons(1);
    memcpy(data_header,&d_packet,sizeof(d_packet));
    memcpy(data_response,data_header,sizeof(data_header));
    memcpy(data_payload,buffer,1024);
    memcpy(data_response+12,data_payload,1024);
    ssize_t bytes = send(sock,data_response,1036,0);
    if(bytes == 0)
      printf("Not Sending\n");
    while(bytes != 1036)
      bytes += send(sock,data_response+bytes, 1036-bytes, 0);
  }

  //Response to Controller
  uint16_t payload,response_len;
  char *cntrl_response_header,*cntrl_response_payload,*cntrl_response;
  payload = 0;
  cntrl_response_payload = (char*)malloc(sizeof(char)*payload);
  cntrl_response_header = create_response_header(sock_index,5,0,payload);
  response_len = CNTRL_RESP_HEADER_SIZE + payload;
  cntrl_response = (char*)malloc(response_len);
  memcpy(cntrl_response,cntrl_response_header,CNTRL_RESP_HEADER_SIZE);
  free(cntrl_response_header);
  sendALL(sock_index,cntrl_response,response_len);
  free(cntrl_response);
}
