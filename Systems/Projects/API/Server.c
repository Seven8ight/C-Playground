#include "Header.h"

int main()
{
    Server *server = createServer(3030);
    fd_set *allConnections = malloc(sizeof(fd_set) * 100),
           *readFds = malloc(sizeof(fd_set) * 100);
    int fd_max = server->fd;

    FD_ZERO(allConnections);
    FD_ZERO(readFds);

    while (1)
    {
        acceptConnections(server, allConnections, 100);
    }

    return 0;
}