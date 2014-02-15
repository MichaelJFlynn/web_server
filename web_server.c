#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <pthread.h>

typedef struct{
  int connection;
  struct sockaddr_in client;
} thread_arg;

void *handleConnection(void*);

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
    close(sock);
    return -1;
  }
  
  if(listen(sock, 15) < 0) {
    printf("error listening to socket");
    close(sock);
    return -1;
  }

  // blocking accept function
  // todo: spawn new thread to handle connection
  len = sizeof(cliaddr);
  int i = 0;
  pthread_t tid;
  while(i < 5) {
    int connection = accept(sock, (struct sockaddr *) &cliaddr, &len);
    if(connection == -1){
      printf("Connection failed");
    }
    struct thread_arg arg = (thread_arg) *malloc(sizeof(thread_arg));
    arg.connection = connection;
    arg.client = cliaddr;
    int thread = pthread_create(&tid, NULL, handleConnection, &arg);
    i++;
  } 
  close(sock);
  

  return 0;
}


void * handleConnection(void * arg) {
  struct thread_arg args = (thread_arg) *arg;
  int connection = args.connection;
  struct sockaddr_in cliaddr = args.client;



  printf("Connection from %s, port %d\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
  write(connection, "Hello I am Mike\n", sizeof("Hello I am Mike\n"));
  close(connection);


  free(arg);
  return(NULL);
}