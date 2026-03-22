#include "../Header.h"

int main()
{
    sockaddr_in sa;
    int server_fd, client_fd, status;
    __uint16_t port = 3030;

    sockaddr_storage client_addr;
    socklen_t addr_size;

    memset(&sa, 0, sizeof sa);
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    sa.sin_port = htons(port);

    server_fd = socket(sa.sin_family, SOCK_STREAM, 0);
    status = bind(server_fd, (struct sockaddr *)&sa, sizeof sa);

    if (status != 0)
    {
        fprintf(stderr, "Bind: %s\n", strerror(errno));
        return 2;
    }

    listen(server_fd, BACKLOG);

    addr_size = sizeof client_addr;
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_size);

    if (client_fd == -1)
    {
        fprintf(stderr, "Accept: %s\n", strerror(errno));
        return 3;
    }

    printf("New connection! Socket fd: %d, client fd: %d\n", server_fd, client_fd);

    return 0;
}