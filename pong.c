/*
 * pong.c - UDP ping/pong server code
 *          author: Paul Fridman
 */
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "util.h"

#define PORTNO "1266"


int main(int argc, char **argv) {
  int ch;
  int nping = 1;                    // default packet count
  char *pongport = strdup(PORTNO);  // default port

  while ((ch = getopt(argc, argv, "h:n:p:")) != -1) {
    switch (ch) {
    case 'n':
      nping = atoi(optarg);
      break;
    case 'p':
      pongport = strdup(optarg);
      break;
    default:
      fprintf(stderr, "usage: pong [-n #pings] [-p port]\n");
    }
  }

  // pong implementation goes here.
  printf("nping: %d pongport: %s\n", nping, pongport);

  int sockfd = -1;
  struct addrinfo hints, *servinfo, *p;
  int rv, yes;

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_PASSIVE;

  if((rv = getaddrinfo(NULL, pongport, &hints, &servinfo)) != 0){
    perror("getaddrinfo");
    return 1;
  }

  for(p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype,
            p->ai_protocol)) == -1) {
        perror("server: socket");
        continue;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
            sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
        close(sockfd);
        perror("server: bind");
        continue;
    }

    break;
  }

  freeaddrinfo(servinfo);

  struct sockaddr_storage from_addr;

  if(p == NULL){
    perror("server failed bind\n");
    close(sockfd);
    exit(1);
  }
  char buf[2000];
  char s[INET_ADDRSTRLEN];
  socklen_t from_len = sizeof(from_addr);
  for(int i = 0; i < nping; i++){
    ssize_t arraysize = recvfrom(sockfd, buf, 2000, 0, 
                      (struct sockaddr *) &from_addr, &from_len);
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)&from_addr;
    inet_ntop(AF_INET, &(ipv4->sin_addr), s, sizeof s);
    printf("pong[%d]: received packet from %s\n", i, s);
    for(int el = 0; el < arraysize; el++){
      buf[el] += 1;
    }
    sendto(sockfd, buf, arraysize, 0, (struct sockaddr *) &from_addr, from_len);
  }
  
  return 0;
}

