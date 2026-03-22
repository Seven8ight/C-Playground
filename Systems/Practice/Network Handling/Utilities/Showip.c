#include "../Header.h"

int main(int argc, char const **argv)
{
    addrinfo hints, *res, *r;
    int status;
    char buffer[INET6_ADDRSTRLEN];

    if (argc != 2)
    {
        fprintf(stderr, "usage: /a hostname\n");
        return 1;
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    status = getaddrinfo(argv[1], 0, &hints, &res);
    if (status != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 2;
    }

    printf("IP addresses for %s:\n", argv[1]);

    r = res;

    while (r != NULL)
    {
        void *addr;

        if (r->ai_family == AF_INET)
        {
            sockaddr_in *ipv4 = (sockaddr_in *)r->ai_addr;
            inet_ntop(r->ai_family, &(ipv4->sin_addr), buffer, sizeof(buffer));
            printf("IPv4: %s\n", buffer);
        }
        else
        {
            sockaddr_in6 *ipv6 = (sockaddr_in6 *)r->ai_addr;
            inet_ntop(r->ai_family, &(ipv6->sin6_addr), buffer, sizeof(buffer));
            printf("IPv6 %s\n", buffer);
        }

        r = r->ai_next;
    }

    freeaddrinfo(res);
    return 0;
}