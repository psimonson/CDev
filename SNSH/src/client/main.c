/*********************************************************
 * main.c - SNSH Client main source file.
 *********************************************************
 * Created by Philip "5n4k3" Simonson
 *********************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "../debug.h"
#include "../prs_socket/socket.h"

int hdl_client(int *, struct sockaddr_in *, const char *);

int main(int argc, char *argv[]) {
  sockcreate_func_t sockfunc;
  struct sockaddr_in client;
  int sockfd, clientfd, retval;

  if(argc < 2 || argc > 3) {
    printf("Usage: %s <ipaddress> [port]\n", argv[0]);
    return 1;
  }

  if(argc == 2) {
    ERROR_FIXED(socket_init(SOCKET_CONN, &sockfunc) < 0, "socket init failed.\n");
    ERROR_FIXED((sockfd = create_conn(argv[1], 8888, &clientfd, &client)) < 0,
		"Could not create socket.\n");
    ERROR_FIXED(handle_server(&sockfd, &clientfd, &client, NULL, &hdl_client) < 0,
		"Could handle server.\n");
    retval = hdl_client(&sockfd, &client, NULL);
  } else {
    ERROR_FIXED(socket_init(SOCKET_CONN, &sockfunc) < 0, "socket init failed.\n");
    ERROR_FIXED((sockfd = create_conn(argv[1], atoi(argv[2]), &clientfd, &client)) < 0,
		"Could not create socket.\n");
    ERROR_FIXED(handle_server(&sockfd, &clientfd, &client, NULL, &hdl_client) < 0,
		"Could handle server.\n");
    retval = hdl_client(&sockfd, &client, NULL);
  }
  close_socket(&sockfd);
  return retval;

 error:
  close_socket(&sockfd);
  return 1;
}

int hdl_client(int *sockfd, struct sockaddr_in *client, const char *filename) {
  char msg[BUFSIZ];
  int bytes;
  
  do {
    ERROR_FIXED((bytes = recv(*sockfd, msg, sizeof msg, 0)) < 0, "Could not recv message.\n");
  } while(bytes > 0);
  
  return 0;

 error:
  close_socket(sockfd);
  return 1;
}
