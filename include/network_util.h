#ifndef NETWORK_UTIL_H_
#define NETWORK_UTIL_H_
#include <netinet/in.h>
#include "../include/control_header_lib.h"

ssize_t recvALL(int sock_index, char *buffer, ssize_t nbytes);
ssize_t sendALL(int sock_index, char *buffer, ssize_t nbytes);
void sendUDP(int sock_index, struct UPDATE_VECTOR *temp,int n_bytes,struct sockaddr_in *addr,socklen_t addr_len);
#endif
