#ifndef CONTROL_HANDLER_H_
#define CONTROL_HANDLER_H_

int create_control_sock();
int new_control_conn(int sock_index);
bool isControl(int sock_index);
bool control_recv_hook(int sock_index);
bool isData(int sock_index);
int new_data_conn(int sock_index);
bool recv_data(int sock_index);

#endif
