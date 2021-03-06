/******************************************************
 * A program to poweroff multiple computers across
 * a network.
 ******************************************************
 */

#define _WIN32_WINNT 0x0500

#if defined(__linux__)
	#include <sys/socket.h>
	#include <arpa/inet.h>
#elif defined(_WIN32)
	#include <windows.h>
	#include <winsock2.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define error(T) if((T)){ fprintf(stderr, "%s\n", strerror(errno)); errno = 0; goto error; }

void (*funcThread)(int clientfd);

#if defined(_WIN32)
static BOOL hide_wnd(HWND wnd, BOOL bShow)
{
	return (bShow) ? ShowWindow(wnd, SW_SHOW) : ShowWindow(wnd, SW_HIDE);
}
#endif

void prog_thread(int clientfd)
{
#if defined(_WIN32)
    char *pname = "C:\\Windows\\system32\\cmd.exe";
	const char *args[] = {"/c", "shutdown", "/s", NULL};
	hide_wnd(GetConsoleWindow(), FALSE);
#elif defined(__linux__)
	const char *pname = "/bin/sh";
	char *const args[] = {"-c", "shutdown", "-P", "now", NULL};
#endif
	execvp(pname, args);
}

int main()
{
	struct sockaddr_in self;
	int sockfd;

#if defined(_WIN32)
	WSADATA wsaData;
	ZeroMemory(&wsaData, sizeof(wsaData));
	WSAStartup(MAKEWORD(2, 0), &wsaData);
#endif

	if((sockfd=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket()");
#if defined(_WIN32)
		WSACleanup();
#endif
		exit(1);
	}
#if defined(__linux__)
	bzero(&self, sizeof(self));
#elif defined(_WIN32)
	ZeroMemory(&self, sizeof(self));
#endif
	self.sin_family = AF_INET;
	self.sin_port = htons(8888);
	self.sin_addr.s_addr = INADDR_ANY;

    error(bind(sockfd, (struct sockaddr*)&self, sizeof(self)) < 0);
    error(listen(sockfd, 20) < 0);

    while(1) {
        struct sockaddr_in client;
        int clientlen = sizeof(client);
        int clientfd;

		clientfd=accept(sockfd, (struct sockaddr*)&client, &clientlen);

		if(clientfd > 0) {
#if defined(_WIN32)
			funcThread = &prog_thread;
			funcThread(clientfd);
			closesocket(clientfd);
#elif defined(__linux__)
			funcThread = &prog_thread;
			funcThread(clientfd);
			close(clientfd);
#endif
		} else
			perror("accept()");
    }

#if defined(_WIN32)
	closesocket(sockfd);
	WSACleanup();
#elif defined(__linux__)
	close(sockfd);
#endif
    return 0;

error:
#if defined(_WIN32)
	closesocket(sockfd);
	WSACleanup();
#elif defined(__linux__)
	close(sockfd);
#endif
	return 1;
}
