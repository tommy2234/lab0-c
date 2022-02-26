#ifndef __TINY_H__
#define __TINY_H__

#include <arpa/inet.h> /* inet_ntoa */
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

typedef struct sockaddr SA;

typedef struct {
    char function_name[512];
    off_t offset; /* for support Range */
    size_t end;
} http_request;

extern int listenfd;

int get_listenfd(int argc, char **argv);
void parse_request(int fd, http_request *req);

// get the function_name
char *process(int fd, struct sockaddr_in *clientaddr);

#endif
