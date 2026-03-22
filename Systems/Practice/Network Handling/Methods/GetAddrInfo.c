#include "../Header.h"

int main(int argc, char *args[])
{
    addrinfo hints, *dnsResults;
    int server_fd, client_fd, status;

    sockaddr_storage client_addr;
    socklen_t addr_size;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    status = getaddrinfo(NULL, PORT, &hints, &dnsResults);

    if (status != 0)
    {
        fprintf(stderr, "Error on retrieving address information: %s\n", gai_strerror(status));
        return 1;
    }

    server_fd = socket(dnsResults->ai_family, dnsResults->ai_socktype, dnsResults->ai_protocol);
    status = bind(server_fd, dnsResults->ai_addr, dnsResults->ai_addrlen);

    if (status != 0)
    {
        fprintf(stderr, "Bind error: %s\n", strerror(errno));
        return 2;
    }

    listen(server_fd, BACKLOG);

    addr_size = sizeof client_addr;
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_size);

    if (client_fd == -1)
    {
        fprintf(stderr, "accept: %s\n", strerror(errno));
        return 3;
    }

    printf("New connection! Socket fd: %d, client fd: %d\n", server_fd, client_fd);

    return 0;
}