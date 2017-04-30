/*
 *****************************************************
 * A simple socket program to capture input from user.
 * by Philip R. Simonson.
 *****************************************************
 */

#if defined(__linux__)
#include <sys/socket.h>
#include <arpa/inet.h>
#else
#include <winsock2.h>
#endif /* linux  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define assert(T, M) ({\
		if((T)) {\
		fprintf(stderr, "%s",\
			(M == NULL) ? strerror(errno) : M);\
		errno = 0;\
		goto error;\
		}\
	})

#define PORT 8888
#define BUFSIZE 1

char*
getln_remote(socket, a)
const int *socket;
unsigned char a;
{
	char *buf=NULL;
	char c;
	int size, p;

	p=0;
	size=BUFSIZE;
	buf = realloc(buf, size);
	assert(buf==NULL,NULL);

	while(1) {
#if defined(__linux__)
		read(*socket, &c, 1);
#elif defined(_WIN32)
		recv(*socket, &c, 1, 0);
#endif
		if(c==-1 || c==0x0a || c==0x0d) {
			if(p>=size && a) {
				size += BUFSIZE+1;
				buf=realloc(buf, size);
				assert(buf==NULL,NULL);
				*(buf+p)=0x0a;
				*(buf+p+1)=0x00;
			} else {
				*(buf+p)=0x00;
			}
			return(buf);
		} else {
			*(buf+p)=c;
#if defined(_WIN32)
			send(*socket, &c, 1, 0);
#endif
		}
		++p;

		if(p>=size) {
			size+=BUFSIZE;
			buf=realloc(buf,size);
			assert(buf==NULL, "Error: Out of memory.");
		}
	}

error:
	return(NULL);
}

void
pzero(p, s)
char* p;
size_t s;
{
	size_t i;
	for(i=0; i<s; ++i)
		*p=0;
}

int
main(argc,argv)
int argc;
char **argv;
{
	int sockfd;
	struct sockaddr_in self;
	char *buf;

#if defined(_WIN32)
	WSADATA wsaData;

	/* WSAStartup() */
	WSAStartup(MAKEWORD(2,0), &wsaData);
#endif

	if((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Socket");
		exit(errno);
	}

	/*---Initialize address/port struct---*/
	pzero(&self, sizeof(self));
	self.sin_family = AF_INET;
	self.sin_port = htons(PORT);
	self.sin_addr.s_addr = INADDR_ANY;

	/*---Assign a port number to the socket---*/
	if(bind(sockfd, (struct sockaddr*)&self, sizeof(self)) != 0) {
		perror("socket--bind()");
		exit(errno);
	}

	/*---Make it a listening socket---*/
	if(listen(sockfd, 20) != 0) {
		perror("socket--listen()");
		exit(errno);
	}

	/*---Forever...---*/
	for(;;) {
		int clientfd;
		struct sockaddr_in client_addr;
		int addrlen=sizeof(client_addr);
		char message[256];

		pzero(message, sizeof(message));
		clientfd=accept(sockfd, (struct sockaddr*)&client_addr, &addrlen);
		printf("%s:%d connected\n", inet_ntoa(client_addr.sin_addr),
				ntohs(client_addr.sin_port));

		/*---Capture stuff---*/
		buf=getln_remote(&clientfd, 1);
		send(clientfd, buf, strlen(buf), 0);
		sprintf(message, "%s: %s\n", inet_ntoa(client_addr.sin_addr),
				buf);
		puts(message);
		free(buf);

		/*---Close data connection---*/
#if defined(_WIN32)
		closesocket(clientfd);
#else
		close(clientfd);
#endif
	}

	if(buf!=NULL) free(buf);
#if defined(_WIN32)
	closesocket(sockfd);
	WSACleanup();
#else
	close(sockfd);
#endif
	return 0;
}
