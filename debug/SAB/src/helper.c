#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>

#include "socket.h"
#include "../../debug.h"
#include "helper.h"

#ifdef __linux__
#include <sys/stat.h>
#include <espeak/speak_lib.h>
#endif

#include "espeech.h"

char *builtin_str[] = {
  "ls",
  "rm",
  "mkdir",
  "rmdir",
  "touch",
  "speak",
  "help",
  "exit"
};

char *builtin_help[] = {
  "list directory contents.\r\n",
  "delete a file from the system.\r\n",
  "make a directory in the current one.\r\n",
  "delete an empty directory.\r\n",
  "create a blank file.\r\n",
  "speaks the text you type.\r\n",
  "print this message.\r\n",
  "exit back to echo hello name.\r\n"
};

int cmd_len(void);

int cmd_ls(int sockfd, char **args) {
  DIR *d;
  struct dirent *dir;
  char msg[1024];
  int i;
  
  memset(msg, 0, sizeof msg);
  if(args[0] == NULL)
    return 1;

  if(args[1] != NULL)
    d = opendir(args[1]);
  else
    d = opendir(".");
  
  if(d != NULL) {
    memset(msg, 0, sizeof msg);
    if(args[1] != NULL)
      snprintf(msg, sizeof msg, "Directory listing of %s\r\n", args[1]);
    else
      snprintf(msg, sizeof msg, "Directory listing of ./\r\n");
    if(send(sockfd, msg, strlen(msg), 0) < 0)
      puts("Error: Could not send data to client.");
    i = 0;
    while((dir = readdir(d))) {
      memset(msg, 0, sizeof msg);
      snprintf(msg, sizeof msg, "%s ", dir->d_name);
      send(sockfd, msg, strlen(msg), 0);
      if(i < 4)
	++i;
      else {
	i = 0;
	if(send(sockfd, "\r\n", 2, 0) != 2)
	  puts("Error: Could not send data to client.");
      }
    }
    if(send(sockfd, "\r\n", 2, 0) != 2)
      puts("Error: Could not send data to client.");
    memset(msg, 0, sizeof msg);
    if(closedir(d) != 0) {
      snprintf(msg, sizeof msg, "End of listing.\r\n");
      if(send(sockfd, msg, strlen(msg), 0) < 0)
	puts("Error: Could not send data to client.");
    }
  } else {
    snprintf(msg, sizeof msg, "Could not list directory, maybe it doesn't exist.\r\n");
    if(send(sockfd, msg, strlen(msg), 0) < 0)
      puts("Error: Could not send data to client.");
  }
  return 1;
}

int cmd_rm(int sockfd, char **args) {
  char data[BUFSIZ];
  int i = 1;
  
  if(args == NULL || sockfd < 0)
    return -1;
  else if(args[1] == NULL)
    return -1;
  else {
    char msg[BUFSIZ];

    while(args[i] != NULL) {
      if(remove(args[i]) != 0) {
	memset(msg, 0, sizeof msg);
	snprintf(msg, sizeof msg, "Cannot remove file %s\n", args[i]);
	ERROR_FIXED(send(sockfd, msg, strlen(msg), 0) < 0, "Could not send message.");
      } else {
	snprintf(msg, sizeof msg, "File %s removed.\r\n", args[i]);
	ERROR_FIXED(send(sockfd, msg, strlen(msg), 0) < 0, "Could not send message.");
      }
      ++i;
    }
  }
  snprintf(data, sizeof data, "Total files removed %d.\r\n", i-1);
  ERROR_FIXED(send(sockfd, data, strlen(data), 0) < 0, "Could not send message.");
  return 1;

error:
  return 1;
}

int cmd_mkdir(int sockfd, char **args) {
  char data[BUFSIZ];
  
  if(args == NULL || sockfd < 0)
    return -1;

  memset(data, 0, sizeof data);
  snprintf(data, sizeof data, "Command not yet implemented!\r\n");
  ERROR_FIXED(send(sockfd, data, strlen(data), 0) < 0, "Could not send data to client.");
  return 1;

error:
  return 1;
}

int cmd_rmdir(int sockfd, char **args) {
  char data[BUFSIZ];
  
  if(args == NULL || sockfd < 0)
    return -1;

  memset(data, 0, sizeof data);
  snprintf(data, sizeof data, "Command not yet implemented!\r\n");
  ERROR_FIXED(send(sockfd, data, strlen(data), 0) < 0, "Could not send data to client.");
  return 1;

error:
  return 1;
}

int cmd_touch(int sockfd, char **args) {
  char data[BUFSIZ];
  int i = 1;
  
  if(args == NULL || sockfd < 0)
    return -1;
  else if(args[1] == NULL)
    return -1;
  else {
    while(args[i] != NULL) {
      FILE *fp = NULL;
      memset(data, 0, sizeof data);
      if((fp = fopen(args[i], "wb")) == NULL) {
	snprintf(data, sizeof data, "File [%s] failed to create.\r\n", args[i]);
	ERROR_FIXED(send(sockfd, data, strlen(data), 0) < 0, "Could not send data to client.");
      } else {
	snprintf(data, sizeof data, "File [%s] created successfully.\r\n", args[i]);
	ERROR_FIXED(send(sockfd, data, strlen(data), 0) < 0, "Could not send data to client.");
      }
      fclose(fp);
      ++i;
    }
  }
  memset(data, 0, sizeof data);
  snprintf(data, sizeof data, "Total count of files created: %d\r\n", i-1);
  ERROR_FIXED(send(sockfd, data, strlen(data), 0) < 0, "Could not send data to client.");
  return 1;

error:
  return 1;
}

int cmd_speak(int sockfd, char **args) {
  char data[2048];
  char msg[1024];
  int i = 1;

  memset(msg, 0, sizeof msg);
  memset(data, 0, sizeof data);
  if(args == NULL || sockfd < 0)
    return -1;

  snprintf(data, sizeof data, "Speaking: ");
  strncpy(msg, args[i], sizeof msg);
  while(args[++i] != NULL) {
    strncat(msg, " ", sizeof msg);
    strncat(msg, args[i], sizeof msg);
  }
  strncat(data, msg, sizeof data);
  strncat(data, "\r\n", sizeof data);
  ERROR_FIXED(send(sockfd, data, strlen(data), 0) < 0, "Could not send data to client.");
  /* Speak the message */
#ifdef __linux__
  speak(msg, strlen(msg));
#endif
  return 1;

error:
  return 1;
}

int cmd_help(int sockfd, char **args) {
  char msg[BUFSIZ];
  int i;

  memset(msg, 0, sizeof msg);
  if(args[0] == NULL || sockfd < 0)
    return -1;

  snprintf(msg, sizeof msg, "*** Help Below ***\r\n");
  for(i = 0; i < cmd_len(); i++) {
    strncat(msg, builtin_str[i], sizeof msg);
    strncat(msg, " - ", sizeof msg);
    strncat(msg, builtin_help[i], sizeof msg);
  }
  if(send(sockfd, msg, strlen(msg), 0) < 0)
    puts("Error: Could not send data to client.");
  
  return 1;
}

int cmd_exit(int sockfd, char **args) {
  if(args[0] == NULL || sockfd == 0)
    return 1;

  return 0;
}

int (*builtin_func[])(int sockfd, char **args) = {
  &cmd_ls,
  &cmd_rm,
  &cmd_mkdir,
  &cmd_rmdir,
  &cmd_touch,
  &cmd_speak,
  &cmd_help,
  &cmd_exit
};

int cmd_len(void) {
  return sizeof(builtin_str) / sizeof(char *);
}

char **cmd_split(char line[]) {
  char **tokens = NULL;
  char *token = NULL;
  int i = 0, size;

  size = CMD_TOK_SIZE * sizeof(char *);
  tokens = (char **)malloc(size);
  CHECK_MEM(tokens);

  token = strtok(line, CMD_TOK_DELIMS);
  while(token != NULL) {
    tokens[i] = token;
    ++i;
    if(i >= size) {
      size += CMD_TOK_SIZE * sizeof(char *);
      tokens = (char **)realloc(tokens, size);
      CHECK_MEM(tokens);
    }
    token = strtok(NULL, CMD_TOK_DELIMS);
  }
  tokens[i] = NULL;
  return tokens;
  
error:
  return NULL;
}

int cmd_execute(int sockfd, char **args) {
  char data[BUFSIZ];
  int i;

  memset(data, 0, sizeof(data));
  if(args == NULL) {
    snprintf(data, sizeof(data), "Error: No arguments given.\r\n");
    if(send(sockfd, data, strlen(data), 0) < 0)
      puts("Error: Could not receive data from client.");
    return 1;
  } else if(args[0] == NULL) {
    snprintf(data, sizeof data, "Error: No arguments given.\r\n");
    if(send(sockfd, data, strlen(data), 0) < 0)
      puts("Error: Could not receive data from client.");
    return 1;
  }

  for(i = 0; i < cmd_len(); i++)
    if(strcmp(args[0], builtin_str[i]) == 0)
      return (*builtin_func[i])(sockfd, args);

  snprintf(data, sizeof data, "Error: Command not found.\r\n");
  if(send(sockfd, data, strlen(data), 0) < 0)
    puts("Error: Cannot send data to client.");
  return 1;
}

void cmd_loop(int *sockfd, struct sockaddr_in *client) {
  char line[CMD_LEN];
  char msg[1024];
  char **args = NULL;
  int status;

  if(client == NULL)
    return;

#ifdef __linux__
  speakInit();
#endif
  
  do {
    memset(line, 0, sizeof line);
    memset(msg, 0, sizeof msg);
    snprintf(msg, sizeof msg, "CMD > ");
    if(send(*sockfd, msg, strlen(msg), 0) < 0)
      puts("Error: Cannot send message to client.");
    if(recv(*sockfd, line, sizeof line, 0) < 0)
      puts("Error: Cannot recv from client.");
    args = cmd_split(line);
    status = cmd_execute(*sockfd, args);
    free(args);
  } while(status);
#ifdef __linux__
  speakCleanup();
#endif
}
