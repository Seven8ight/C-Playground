#include "../../Header.h"

int main(int argc, const char *args[])
{
    if (argc <= 1)
    {
        printf("Provide a domain alongside execution");
        return -1;
    }

    addrinfo hints, **res;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int getAddress = getaddrinfo(args[1], 0, &hints, res);

    if (getAddress != 0)
    {
        fprintf(stderr, "Get address error: %s", gai_strerror(getAddress));
        return -2;
    }

    char buffer[BUFSIZ];

    while (*res != NULL)
    {
        if ((*res)->ai_family == AF_INET)
        {
            sockaddr_in *ipv4 = (sockaddr_in *)(*res)->ai_addr;

            inet_ntop(AF_INET, &ipv4->sin_addr, buffer, BUFSIZ);
            printf("Ip address of specified domain is: %s\n", buffer);
        }
        else
        {
            sockaddr_in6 *ipv6 = (sockaddr_in6 *)(*res)->ai_addr;

            inet_ntop(AF_INET6, &(ipv6->sin6_addr), buffer, BUFSIZ);
            printf("Ip address of specified domain is: %s\n", buffer);
        }

        *res = (*res)->ai_next;
    }

    freeaddrinfo(*res);
    return 0;
}