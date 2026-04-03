#ifndef LIB_H
#define LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

#define BACKLOG 100

typedef enum
{
    IPv4,
    IPv6
} IPType;

typedef struct
{
    int fd;
    IPType type;
    struct addrinfo *serverInfo;
} Server;

typedef struct addrinfo addrinfo;

Server *createServer(int port);
FILE *fileOpener(char *sourcePath);
char *fileContents(FILE *file);
void acceptConnections(Server *server, fd_set *allFds, int *fd_max);

#endif