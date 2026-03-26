#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

class DNSResolver
{
public:
    DNSResolver(const std::string &hostname) : host(hostname) {}

    void resolve()
    {
        addrinfo hints{}, *res = nullptr;

        hints.ai_family = AF_UNSPEC;     // IPv4 or IPv6
        hints.ai_socktype = SOCK_STREAM; // TCP stream

        int status = getaddrinfo(host.c_str(), nullptr, &hints, &res);
        if (status != 0)
        {
            std::cerr << "getaddrinfo error: " << gai_strerror(status) << "\n";
            return;
        }

        // Ensure resources are freed automatically
        struct AddrInfoDeleter
        {
            void operator()(addrinfo *p) { freeaddrinfo(p); }
        };
        std::unique_ptr<addrinfo, AddrInfoDeleter> resPtr(res);

        std::cout << "IP addresses for " << host << ":\n";

        for (addrinfo *r = res; r != nullptr; r = r->ai_next)
        {
            char buffer[INET6_ADDRSTRLEN];

            if (r->ai_family == AF_INET)
            {
                sockaddr_in *ipv4 = reinterpret_cast<sockaddr_in *>(r->ai_addr);
                inet_ntop(r->ai_family, &(ipv4->sin_addr), buffer, sizeof(buffer));
                std::cout << "IPv4: " << buffer << "\n";
            }
            else if (r->ai_family == AF_INET6)
            {
                sockaddr_in6 *ipv6 = reinterpret_cast<sockaddr_in6 *>(r->ai_addr);
                inet_ntop(r->ai_family, &(ipv6->sin6_addr), buffer, sizeof(buffer));
                std::cout << "IPv6: " << buffer << "\n";
            }
        }
    }

private:
    std::string host;
};

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " hostname\n";
        return 1;
    }

    DNSResolver resolver(argv[1]);
    resolver.resolve();

    return 0;
}