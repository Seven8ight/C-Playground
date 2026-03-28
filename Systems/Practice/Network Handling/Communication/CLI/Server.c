#include "../../Header.h"

typedef struct
{
    int socket_fd;
    struct sockaddr socketAddrInfo;
} Socket;

typedef enum
{
    IPV4,
    IPV6
} AddrType;

typedef enum
{
    CLIENT,
    SERVER
} ConnType;

int createSocket(AddrType ipType, ConnType connectionType)
{
}

int main();