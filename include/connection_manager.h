#ifndef CONNECTION_MANAGER_H_
#define CONNECTION_MANAGER_H_

#include <sys/select.h>

int control_socket, router_socket, data_socket;
struct timeval tv;
fd_set master_list;
int head_fd;
void init();

#endif
