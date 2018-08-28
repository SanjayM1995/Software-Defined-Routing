/**
 * @connection_manager
 * @author  Sanjay Muthu Manickam
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
 * Connection Manager listens for incoming connections/messages from the
 * controller and other routers and calls the desginated handlers.
 */

#include <sys/select.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/time.h>

#include "../include/connection_manager.h"
#include "../include/global.h"
#include "../include/control_handler.h"
#include "../include/send_update.h"
#include "../include/init_response.h"
#include "../include/recv_from.h"
#include "../include/recv_file.h"

fd_set watch_list;

void recv_file();

void main_loop()
{
    int selret, sock_index, fdaccept;
    extern int head_fd;
    extern int update_interval;
    int counter;
    int time_out;
    counter = 0;
    tv.tv_sec = 65535;
    int fl =0;
    struct timeval *curr_time;
    // printf("TIME IS : %i\n",tv.tv_sec);
    while(true){
        watch_list = master_list;
        // printf("TIME IS : %i\n",tv.tv_sec);
        selret = select(head_fd+1, &watch_list,NULL,NULL,&tv);
        if(selret == 0)
        {
          send_update(1,0);
          tv.tv_sec = update_interval;
        }
        else
        {
        /* Loop through file descriptors to check which ones are ready */
          for(sock_index=0; sock_index<=head_fd; sock_index+=1)
          {
                if(FD_ISSET(sock_index, &watch_list))
                {

                  /* control_socket */
                  if(sock_index == control_socket)
                  {
                    fdaccept = new_control_conn(sock_index);

                    /* Add to watched socket list */
                    FD_SET(fdaccept, &master_list);
                    if(fdaccept > head_fd)
                    head_fd = fdaccept;
                  }
                  /* router_socket */
                  else if(sock_index == router_socket)
                  {
                    //call handler that will call recvfrom() .....
                    recv_from();
                  }

                  /* data_socket */
                  else if(sock_index == data_socket)
                  {
                        fdaccept = new_data_conn(sock_index);
                        FD_SET(fdaccept,&master_list);
                        if(fdaccept>head_fd)
                          head_fd = fdaccept;

                  }
                  /* Existing connection */
                  else
                  {
                    if(isControl(sock_index))
                    {
                    if(!control_recv_hook(sock_index)) FD_CLR(sock_index, &master_list);
                    // if(counter==2)
                    // {
                    //   time_out = update_interval;
                    // }
                    }
                    else if (isData(sock_index))
                    {
                        if(!recv_data(sock_index)) FD_CLR(sock_index,&master_list);
                    }
                    else
                    {
                      printf("SOCK : %i\n",sock_index);
                      ERROR("Unknown socket index : ");
                    }
                  }
                }
              }
            }
    }
}

void init()
{
    control_socket = create_control_sock();
    extern int head_fd;
    //router_socket and data_socket will be initialized after INIT from controller
    FD_ZERO(&master_list);
    FD_ZERO(&watch_list);
    /* Register the control socket */
    FD_SET(control_socket, &master_list);
    head_fd = control_socket;
    main_loop();
}
