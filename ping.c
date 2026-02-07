/*
 * ping.c - UDP ping/pong client code
 *          author: Paul Fridman
 */
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "util.h"

#define PORTNO "1266"


int main(int argc, char **argv) {
  int ch, errors = 0;
  int nping = 1;                        // default packet count
  char *ponghost = strdup("localhost"); // default host
  char *pongport = strdup(PORTNO);      // default port
  int arraysize = 100;                  // default packet size

  while ((ch = getopt(argc, argv, "h:n:p:s:")) != -1) {
    switch (ch) {
    case 'h':
      ponghost = strdup(optarg);
      break;
    case 'n':
      nping = atoi(optarg);
      break;
    case 'p':
      pongport = strdup(optarg);
      break;
    case 's':
      arraysize = atoi(optarg);
      break;
    default:
      fprintf(stderr, "usage: ping [-h host] [-n #pings] [-p port] [-s size]\n");
    }
  }

  // UDP ping implemenation goes here
  printf("nping: %d arraysize: %d errors: %d ponghost: %s pongport: %s\n",
      nping, arraysize, errors, ponghost, pongport);

  char lst[arraysize];
  memset(lst, 200, arraysize);

  int sockfd = -1;
  struct addrinfo hints, *servinfo, *p;
  int rv;
  
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;

  if((rv = getaddrinfo(ponghost, pongport, &hints, &servinfo)) != 0){
    perror("getaddrinfo");
    return 1;
  }

  for(p = servinfo; p != NULL; p=p->ai_next){
    if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
      perror("client: socket");
      continue;
    }

//    if(connect(sockfd, p->ai_addr, p->ai_addrlen) == -1){
//      close(sockfd);
//      perror("client: connect");
//      continue;
//    }
    break;
  }
  
  char buf[arraysize];
  int errcount = 0;
  double totaltotaltime = 0;
  struct sockaddr_storage from_addr;
  socklen_t from_len;

  for(int i = 0; i < nping; i++){
    from_len = sizeof(from_addr);
    memset(buf, 199, arraysize); //make sure we're not using prev results
    double start = get_wctime();
    
    sendto(sockfd, lst, arraysize, 0, p->ai_addr, p->ai_addrlen);
  
    recvfrom(sockfd, buf, arraysize, 0,(struct sockaddr *) &from_addr, &from_len);

    double end = get_wctime();
    double totaltime = end - start;

    for(int el = 0; el < arraysize; el++){
      int var = (unsigned char)buf[el];
      if(var != 201){
        errcount += 1;
        printf("error detected\n");
        break;
      }
    }
    
    totaltime *= 1000;  //s to ms
    printf("ping[%d] : round-trip time: %.3f ms\n", i, totaltime);
    totaltotaltime += totaltime;
  }
  if(errcount == 0){
    printf("no errors detected\n");
  }else{
    printf("%d errors detected\n", errcount);
  }
  double averagetime = totaltotaltime / nping;
  printf("time to send %d packets of %d bytes: %.3f  (%.3f avg per packet)\n", nping, arraysize, 
                          totaltotaltime, averagetime);


  freeaddrinfo(servinfo);




  return 0;
}
