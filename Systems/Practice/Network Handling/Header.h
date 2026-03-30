#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <unistd.h>
#include <curl/curl.h>
#include <stdbool.h>
#include <ctype.h>

#define HTTP 80
#define BACKLOG 100
#define PORT 4040

typedef struct addrinfo addrinfo;
typedef struct sockaddr_storage sockaddr_storage;
typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr_in6 sockaddr_in6;