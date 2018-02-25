#ifndef SEND_FILE_H_
#define SEND_FILE_H_

void send_file(int sock_index,int payload_len,char* cntrl_payload);

struct __attribute__((__packed__)) FILE_PAYLOAD
{
  uint32_t ip;
  uint8_t ttl;
  uint8_t tid;
  uint16_t seq_num;
  char  *file_name;
};

#endif
