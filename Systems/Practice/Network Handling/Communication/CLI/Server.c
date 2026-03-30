#include "../../Header.h"

typedef enum
{
    IPV4,
    IPV6
} AddrType;

typedef struct
{
    int socket_fd;
    AddrType type;
    sockaddr_in *socketIpv4;
    sockaddr_in6 *socketIpv6;
} Socket;

typedef struct
{
    sockaddr_in *clientsIpv4;
    sockaddr_in6 *clientsIpv6;
    int capacity;
    int noOfClients;
} ClientStorage;

Socket *createSocket(AddrType ipType, int port);
ClientStorage *createClientStorage(AddrType ipType, int noOfClients);
void acceptConnections(Socket *server, ClientStorage *clients, struct pollfd **pollfds, int *pollCount, int *pollSize);
void addToPoll(int serviceFd, struct pollfd **pollfds, int *pollCount, int *pollSize);
void delFromPoll(int serviceFd, struct pollfd **pollfds, int *pollCount, int *pollSize);
void readData(int client_fd, Socket *server, struct pollfd **pollFds, int *pollCount, int *pollSize);
void clearMemory(Socket *server, struct pollfd **pollFds, int *pollCount);

int main(int argc, char *args[])
{
    if (argc <= 1)
    {
        printf("Provide ip type before execution");
        return -1;
    }

    printf("----SERVER----\n");
    AddrType ipType;

    if (strcmp(args[1], "IPV4") == 0)
        ipType = IPV4;
    else
        ipType = IPV6;

    Socket *server = createSocket(ipType, PORT);

    if (server == NULL)
    {
        printf("Insufficient memory\n\n");
        return -1;
    }

    printf("Server is listening at port %d\n", PORT);

    struct pollfd *pollFds = calloc(10, sizeof(struct pollfd));
    int pollCount = 0, pollSize = 10, pollStatus;

    addToPoll(server->socket_fd, &pollFds, &pollCount, &pollSize);

    ClientStorage *clients = createClientStorage(ipType, 10);

    while (true)
    {
        pollStatus = poll(pollFds, pollCount, 2000);

        if (pollStatus == -1)
        {
            fprintf(stderr, "Poll error: %s", strerror(errno));
            break;
        }
        else if (pollStatus == 0)
        {
            printf("Server waiting\n");
            continue;
        }

        for (int i = 0; i < pollCount; i++)
        {
            if (((pollFds + i)->revents & POLLIN) != 1)
                continue;

            if ((pollFds + i)->fd == server->socket_fd)
                acceptConnections(server, clients, &pollFds, &pollCount, &pollSize);
            else
            {
                printf("Client [%d] ready for I/O\n", (pollFds + i)->fd);
                readData((pollFds + i)->fd, server, &pollFds, &pollCount, &pollSize);
            }
        }
    }

    clearMemory(server, &pollFds, &pollCount);

    return 0;
}

ClientStorage *createClientStorage(AddrType ipType, int noOfClients)
{
    ClientStorage *clients = malloc(sizeof(ClientStorage));

    if (!clients)
    {
        perror("Client memory");
        return NULL;
    }

    clients->capacity = 10;
    clients->noOfClients = 0;

    if (noOfClients <= 0)
    {
        printf("No of clients cannot be 0");
        free(clients);
        return NULL;
    }

    if (ipType == IPV4)
    {

        clients->clientsIpv4 = calloc(noOfClients + 1, sizeof clients->clientsIpv4);
        clients->clientsIpv6 = NULL;
    }
    else
    {
        clients->clientsIpv6 = calloc(noOfClients + 1, sizeof clients->clientsIpv6);
        clients->clientsIpv4 = NULL;
    }

    return clients;
}

Socket *createSocket(AddrType ipType, int port)
{
    Socket *connSocket = malloc(sizeof(Socket));
    socklen_t socketSize;
    struct sockaddr *socketInfo;

    if (!connSocket)
    {
        perror("Memory");
        return NULL;
    }

    if (ipType == IPV4)
    {
        connSocket->type = IPV4;
        sockaddr_in ipv4Info = {
            .sin_port = htons(port),
            .sin_family = AF_INET,
            .sin_addr.s_addr = htonl(INADDR_ANY)};

        connSocket->socketIpv4 = &ipv4Info;
        connSocket->socketIpv6 = NULL;
        connSocket->socket_fd = socket(ipv4Info.sin_family, SOCK_STREAM, 0);

        socketInfo = (struct sockaddr *)&ipv4Info;
        socketSize = sizeof ipv4Info;
    }
    else
    {
        connSocket->type = IPV6;
        sockaddr_in6 ipv6Info = {
            .sin6_port = htons(port),
            .sin6_family = AF_INET6,
            .sin6_addr.__u6_addr = htonl(INADDR_ANY)};

        connSocket->socket_fd = socket(ipv6Info.sin6_family, SOCK_STREAM, 0);
        connSocket->socketIpv4 = NULL;
        connSocket->socketIpv6 = &ipv6Info;

        socketInfo = (struct sockaddr *)&ipv6Info;
        socketSize = sizeof ipv6Info;
    }

    if (connSocket->socket_fd == -1)
    {
        fprintf(stderr, "Socket error: %s", strerror(errno));
        free(connSocket);

        return NULL;
    }

    int binding = bind(connSocket->socket_fd, socketInfo, socketSize);
    if (binding != 0)
    {
        fprintf(stderr, "Binding error: %s", strerror(errno));
        free(connSocket);

        return NULL;
    }

    int listening = listen(connSocket->socket_fd, BACKLOG);
    if (listening != 0)
    {
        fprintf(stderr, "Listening error: %s", strerror(errno));
        free(connSocket);

        return NULL;
    }

    return connSocket;
}

void acceptConnections(Socket *server, ClientStorage *clients, struct pollfd **pollfds, int *pollCount, int *pollSize)
{
    if (!server)
    {
        printf("Invalid server socket\n");
        return;
    }

    if (clients->noOfClients >= clients->capacity)
    {
        clients->capacity *= 2;
        if (server->type == IPV4)
        {
            sockaddr_in *clientsIpv4 = realloc(clients->clientsIpv4, sizeof(sockaddr_in) * clients->capacity);
            if (!clientsIpv4)
            {
                printf("Insufficient memory for new clients");
                return;
            }

            clients->clientsIpv4 = clientsIpv4;
        }
        else
        {
            sockaddr_in6 *clientsIpv6 = realloc(clients->clientsIpv6, sizeof(sockaddr_in6) * clients->capacity);
            if (!clientsIpv6)
            {
                printf("Insufficient memory for new clients");
                return;
            }

            clients->clientsIpv6 = clientsIpv6;
        }
    }

    int client_fd;
    socklen_t clientSize = server->type == IPV4 ? sizeof clients->clientsIpv4[0] : sizeof clients->clientsIpv6[0];

    if (server->type == IPV4)
        client_fd = accept(server->socket_fd, (struct sockaddr *)&clients->clientsIpv4[clients->noOfClients++], &clientSize);
    else
        client_fd = accept(server->socket_fd, (struct sockaddr *)&clients->clientsIpv6[clients->noOfClients++], &clientSize);

    if (client_fd == -1)
    {
        fprintf(stderr, "%s", strerror(errno));
        return;
    }

    addToPoll(client_fd, pollfds, pollCount, pollSize);
    printf("------CLIENT------\n");
    printf("Client [%d] joined\n", client_fd);
}

void addToPoll(int serviceFd, struct pollfd **pollfds, int *pollCount, int *pollSize)
{
    if (!pollfds)
    {
        printf("Poll memory corrupted\n");
        return;
    }

    if (*pollCount >= *pollSize)
    {
        *pollSize *= 2;
        struct pollfd *newPolls = realloc(&pollfds, sizeof(struct pollfd) * (*pollSize));
    }

    (*pollfds)[*pollCount].fd = serviceFd;
    (*pollfds)[*pollCount].events = POLLIN;

    (*pollCount) += 1;
}

void delFromPoll(int serviceFd, struct pollfd **pollfds, int *pollCount, int *pollSize)
{
    for (int i = 0; i < *pollCount; i++)
    {
        if ((*pollfds + i)->fd == serviceFd)
        {
            (*pollfds + (i))->fd = (*pollfds + (i - 1))->fd;
            break;
        }
    }

    *(pollCount) -= 1;
}

void readData(int client_fd, Socket *server, struct pollfd **pollFds, int *pollCount, int *pollSize)
{
    if (!server || !pollFds)
    {
        printf("Memory error\n");
        return;
    }

    char buffer[BUFSIZ],
        message[BUFSIZ];
    memset(&buffer, '\0', sizeof buffer);

    int clientMsg = recv(client_fd, buffer, BUFSIZ, 0);
    printf("Message: \"%s\"", buffer);

    if (clientMsg <= 0)
    {
        if (clientMsg == 0)
        {
            printf("Client [%d] left\n", client_fd);
            delFromPoll(client_fd, pollFds, pollCount, pollSize);
            return;
        }
        else if (clientMsg == -1)
        {
            fprintf(stderr, "Client receiving error: %s", strerror(errno));
            return;
        }

        delFromPoll(client_fd, pollFds, pollCount, pollSize);
    }

    sprintf(message, "[%d]:%s", client_fd, buffer);

    for (int i = 0; i < *pollCount; i++)
    {
        if ((*pollFds + i)->events & POLLIN)
        {
            if ((*pollFds + i)->fd != client_fd && (*pollFds + i)->fd != server->socket_fd)
            {
                int destination_fd = (*pollFds + i)->fd;
                int sendMsg = send(destination_fd, message, strlen(message), 0);

                if (sendMsg == -1)
                {
                    fprintf(stderr, "Send error: %s", strerror(errno));
                    return;
                }
            }
        }
    }
}

void clearMemory(Socket *server, struct pollfd **pollFds, int *pollCount)
{
    if (!server || !pollFds)
    {
        printf("Invalid memory\n");
        return;
    }

    if (server->type == IPV4)
        free(server->socketIpv4);
    else
        free(server->socketIpv6);

    free(server);

    for (int i = 0; i < *pollCount; i++)
        free(*pollFds + i);

    free(pollFds);
}