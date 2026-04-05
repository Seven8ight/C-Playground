#include "Header.h"

int main()
{
    signal(SIGPIPE, SigPipe);
    Server *server = createServer(3040);
    fd_set allConnections, readFds;
    int fd_max = server->fd;

    FD_ZERO(&allConnections);
    FD_ZERO(&readFds);
    FD_SET(server->fd, &allConnections);

    while (true)
    {
        struct timeval timer = {
            .tv_sec = 2,
            .tv_usec = 0};

        readFds = allConnections;
        int httpMonitor = select(fd_max + 1, &readFds, NULL, NULL, &timer);

        if (httpMonitor == -1)
        {
            fprintf(stderr, "Select error: %s\n", strerror(errno));
            break;
        }
        else if (httpMonitor == 0)
            continue;
        else
        {
            for (int i = 0; i <= fd_max; i++)
            {
                if (FD_ISSET(i, &readFds))
                {
                    if (i == server->fd)
                        acceptConnections(server, &allConnections, &fd_max);
                    else
                        readSocketData(i, &allConnections, &fd_max);
                }
            }
        }
    }

    return 0;
}