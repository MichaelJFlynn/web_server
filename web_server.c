#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

// HTTP 1.0 status lines
#define HTTP_OK "HTTP/1.0 200 OK\n"
#define HTTP_404 "HTTP/1.0 404 Not Found\n"
#define HTTP_403 "HTTP/1.0 403 Forbidden\n"
#define HTTP_400 "HTTP/1.0 400 Bad Request\n"

// HTTP 1.1 status lines
#define HTTP_11_OK "HTTP/1.1 200 OK\n"
#define HTTP_11_404 "HTTP/1.1 404 Not Found\n"
#define HTTP_11_403 "HTTP/1.1 403 Forbidden\n"
#define HTTP_11_400 "HTTP/1.1 400 Bad Request\n"


struct thread_arg{
  int connection;
  struct sockaddr_in client;
};

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
    struct thread_arg* arg = malloc(sizeof(struct thread_arg));
    arg->connection = connection;
    arg->client = cliaddr;
    int thread = pthread_create(&tid, NULL, handleConnection, arg);
    i++;
  } 
  close(sock);
  

  return 0;
}


void * handleConnection(void * arg) {
  struct thread_arg* args = arg;
  int connection = args->connection;
  struct sockaddr_in cliaddr = args->client;
  char* buffer = malloc(1024);
  char* token;
  int nread, fd;
  printf("Connection from %s, port %d:\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
  while((nread = read(connection, buffer, 1024)) > 0){
    write(1, buffer, nread);
    token = strtok(buffer, " \n\r");
    if(strcmp(token, "GET") == 0) {
      // now token is the filename
      token = strtok(NULL, " \n\r");
      printf("filename: %s", token);
      // sanitize filname
      if(strstr(token, "../") != NULL) {
	// forbidden request
	
      }
      fd = open(token, O_RDONLY);
      if(fd == -1) {
	perror("Error: ");
	write(connection, "404, can't open file", sizeof("404, can't open file"));
	break;
      } else {
	while((nread = read(fd, buffer, sizeof(buffer))) > 0) {
	  token = strtok(NULL, " \n\r");
	  if (strstr(token,"1.0")){
	    write(connection, HTTP_OK, sizeof(HTTP_OK)); 
	  } else {
	    write(connection, HTTP_11_OK, sizeof(HTTP_11_OK));
	  }
	  //write(connection, "Content-Type: %s", );
	  write(connection, buffer, nread);
	}
	close(fd);
	break;
      }
    } else {
      write(connection, "Error: bad request", sizeof("Error: bad request"));
    }
    break;
  }

  close(connection);

  free(buffer);
  free(arg);
  return(NULL);
}


