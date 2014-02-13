#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <pthread.h>

int main(int argc, char ** argv) {

  int fd, nread;
  char buf[1024];
  struct sockaddr_in myaddr, cliaddr;
  int sock;
  socklen_t len;

  
  myaddr.sin_port = htons(8888);
  myaddr.sin_family = AF_INET;
  myaddr.sin_addr.s_addr = htonl(INADDR_ANY);

  sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  
  if(bind(sock, (struct sockaddr *) &myaddr, sizeof(myaddr))< 0) {
    printf("error binding socket");
    return -1;
  }
  
  if(listen(sock, 15) < 0) {
    printf("error listening to socket");
    return -1;
  }

  // blocking accept function
  // todo: spawn new thread to handle connection
  len = sizeof(cliaddr);
  int i = 0;
  while(i < 5) {
    int connection = accept(sock, (struct sockaddr *) &cliaddr, &len);
    printf("Connection from %s, port %d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));

    write(connection, "Hello I am Mike\n", sizeof("Hello I am Mike\n"));
    close(connection);
    i++;
  } 
  close(sock);
  

  return 0;
}
