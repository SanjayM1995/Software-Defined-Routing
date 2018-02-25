/**
 * @network_util
 * @author  Swetank Kumar Saha <swetankk@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * Network I/O utility functions. send/recvALL are simple wrappers for
 * the underlying send() and recv() system calls to ensure nbytes are always
 * sent/received.
 */

#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include "../include/control_header_lib.h"
#include "../include/init_response.h"

extern void print_ip(int);

ssize_t recvALL(int sock_index, char *buffer, ssize_t nbytes)
{
    ssize_t bytes = 0;
    bytes = recv(sock_index, buffer, nbytes, 0);

    if(bytes == 0) return -1;
    while(bytes != nbytes)
        bytes += recv(sock_index, buffer+bytes, nbytes-bytes, 0);

    return bytes;
}

ssize_t sendALL(int sock_index, char *buffer, ssize_t nbytes)
{
    ssize_t bytes = 0;
    bytes = send(sock_index, buffer, nbytes, 0);

    if(bytes == 0) return -1;
    while(bytes != nbytes)
        bytes += send(sock_index, buffer+bytes, nbytes-bytes, 0);

    return bytes;
}

void sendUDP(int sock_index,struct UPDATE_VECTOR *temp,int n_bytes,struct sockaddr_in *addr,socklen_t addr_len)
{
    int errno;
    printf("SENDING : \n");
    struct sockaddr_in new_addr;
    new_addr.sin_family = PF_INET;
    int ip = ntohl(addr->sin_addr.s_addr);
    char ipp[50];
    unsigned char bytes[4];
    bytes[0] = ip & 0xFF;
    bytes[1] = (ip >> 8) & 0xFF;
    bytes[2] = (ip >> 16) & 0xFF;
    bytes[3] = (ip >> 24) & 0xFF;
    sprintf(ipp,"%d.%d.%d.%d", bytes[3], bytes[2], bytes[1], bytes[0]);
    new_addr.sin_addr.s_addr = inet_addr(ipp);
    new_addr.sin_port = htons(addr->sin_port);
    socklen_t addr_len1 = sizeof(new_addr);
    ssize_t bytes1 = 0;
    printf(" DATA : %i\n",temp->port);
    printf("No Of BYTES : %i \n",n_bytes);
    int sock = socket(AF_INET,SOCK_DGRAM,0);
    struct sockaddr_in sock_addr;
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = 0;
    sock_addr.sin_addr.s_addr = INADDR_ANY;
    if(bind(sock,(struct sockaddr*)&sock_addr,sizeof(sock_addr))==-1)
       perror("BIND FAILED");
    int len = no_of_routers*12+8;
    bytes1 = sendto(sock,temp,len,0,(struct sockaddr*)&new_addr,addr_len1);
    if(bytes1==-1)
    {
        perror("Send UDP Error");
    }
    else
    {
        printf("Message Sent %i\n",bytes1);
    }
}
