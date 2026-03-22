# Network Programming

Socket - file descriptor used for remote communication

Types of internet sockets:

1. Stream Sockets -> uses TCP for communication allowing for reliable communication
2. Datagram sockets -> uses UDP for fast communication but without error-checking

Stream sockets use SOCK_STREAM whilst Datagram sockets use SOCK_DGRAM

## SOCK_STREAM

### Importance of byte order

Digits are represented in many forms in computers for machine understanding however
since some digits are big, they are split and stored in separate bytes e.g.
12042 -> 2F0A

2F0A is big hence is split to 2F and 0A, this here is known as big endian representation, the most significant byte is stored first
The opposite is little endian, used by Intel or intel-compatible processors storing the least significant/smallest followed by the largest

Big endian:
12042 -> 2F0A -> 2F 0A
12042 -> 2F0A -> OA 2F

This small difference can be fatal during data transmission if not taken into consideration.

The bytes come as unordered in the network hence have to be reordered appropriately via <arpa/inet.h>
htons -> host to network short -> handles 16 bits/2 bytes
htonl -> host to network long -> handles 32 bits/4 bytes
Opposite of this:
ntohs -> host to network short -> handles 16 bits/2 bytes
ntohl -> host to network long -> handles 32 bits/4 bytes

They either do host byte order to network byte order or vice versa

Both client and server require a connection basis formed from a small data structure containing:

1. The ip address of the service
2. The port the ip address is binded to for connection

netinet/in.h allows us do so, it can be ipv4 (sockaddr_in) or ipv6(sockaddr_in6)

### IPv4 properties:

```C
sockaddr_in{
    sa_family_t sin_family;
    in_port_t sin_port;
    struct in_addr sin addr;
}

struct in_addr{
    uint32_t s_addr;
}
```

1. sa_family_t sin_family -> protocol for the ip address, set appropriately for both ipv4 and ipv6, is only used for ipv4 addresses, value is AF_INET;
2. in_port_t sin_port -> port used for connection/binding, supplied in network byte order not host byte order;
3. struct in_addr sin addr -> contains integer representation of the ipv4 address assigned;

- INADDR_LOOPBACK -> localhost ip address/127.0.0.1
- INADDR_ANY -> 0.0.0.0
- INADDR_BROADCAST -> 255.255.255.255

### IPV6 properties:

```C
struct sockaddr_in6 {
    sa_family_t sin6_family;
    in_port_t sin6_port;
    uint32_t sin6_flowinfo;
    uint32_t sin6_scope_id;
    struct in6_addr sin6_addr;
};

struct in6_addr {
    unsigned char s6_addr[16];
};
```

For ipv6 we get:

1. in6_addr -> 127.0.0.1
2. in6addr_loopback -> 255.255.255.255
3. in6addr_any -> 0.0.0.0

### Converting an IP address to Integer

Ip's are represented as strings not integers
Hence to convert this we use inet_pton() -> pton = presentation to network, that belongs to <arpa/inet.h>

```C
    int inet_pton(int af, const char* src, void* dst);
```

Parameters:

1. af -> protocol specification, AF_INET(IPv4) or AF_INET6(IPv6)
2. src -> string containing the IPv4 or IPv6 address to convert
3. dst -> pointer to store the result as either a IPv4(in_addr) or IPv6(in6_addr)

The function returns 1 on success, 0 when src doesn't contain a valid address and -1 if af is not a valid address family setting errno to EAAFNOSUPPORT

Vice versa of this is inet_ntop() convert the integer rep back to strings.

### Unknown IP addresses

In the event of not knowing ip addresses, we use **getaddrinfo**:

```C
    int getaddrinfo(const char* node, const char* service, const struct addrinfo* hints, struct addrinfo **res);
```

Parameters:

1. Node -> string representation of an IP address, or a domain name .e.g 'www.example.com' NULL is a value too
2. service -> string representation of the port to connect to, a service name
