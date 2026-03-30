#include "../../Header.h"

typedef struct
{
    int socket_fd;
} Client;

Client *createClient(int client_fd)
{
    Client *newClient = malloc(sizeof(Client));

    if (!newClient)
    {
        perror("Memory insufficient for client");
        return NULL;
    }

    newClient->socket_fd = client_fd;

    return newClient;
}

void receiveData(Client *client)
{
    if (!client)
    {
        printf("Client memory corrupted\n");
        return;
    }

    char buffer[BUFSIZ];
    memset(&buffer, '\0', sizeof buffer);

    int bytesReceived = recv(client->socket_fd, buffer, BUFSIZ, 0);
    if (bytesReceived <= 0)
    {
        if (bytesReceived == 0)
        {
            printf("Server closed unexpectedly\n");
            return;
        }
        if (bytesReceived == -1)
        {
            fprintf(stderr, "Data retrieval error: %s", strerror(errno));
            return;
        }
    }
}

void sendData(Client *client, char *message)
{
    if (!client)
    {
        printf("Client memory corrupted\n");
        return;
    }
    if (!message)
    {
        printf("Message cannot be retrieved\n");
        return;
    }

    size_t messageLength = strlen(message);

    if (messageLength == 0)
    {
        printf("No message typed");
        return;
    }

    int sendMessage = send(client->socket_fd, message, messageLength, 0);

    if (sendMessage == -1)
    {
        fprintf(stderr, "Data transmission error: %s", strerror(errno));
        return;
    }
    else
    {
        if (sendMessage != messageLength)
        {
            printf("Message sent incompletely");
            return;
        }
    }
}

int main()
{
    char port[5];
    memset(&port, '\0', sizeof port);
    snprintf(port, sizeof port, "%d", PORT);

    addrinfo hints, *res, *resPtr;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int serverConnection = getaddrinfo("127.0.0.1", port, &hints, &res);

    if (serverConnection != 0)
    {
        fprintf(stderr, "Get address error: %s", gai_strerror(serverConnection));
        return -2;
    }

    fd_set allFds, readFds;
    int fd_max;

    FD_ZERO(&allFds);
    FD_ZERO(&readFds);

    for (resPtr = res; resPtr != NULL; resPtr = resPtr->ai_next)
    {
        int client_fd = socket(resPtr->ai_family, resPtr->ai_socktype, resPtr->ai_protocol);

        if (client_fd == -1)
        {
            fprintf(stderr, "Socket creation error: %s", strerror(errno));
            break;
        }
        int serverConnect = connect(client_fd, (const struct sockaddr *)resPtr->ai_addr, resPtr->ai_addrlen);

        if (serverConnect == -1)
        {
            printf("Failed on client: %d\n", client_fd);
            continue;
        }
        else
        {
            Client *client = createClient(client_fd);
            printf("Connected successfully\nYou are Client fd [%d]\n", client_fd);

            char message[BUFSIZ];
            memset(&message, '\0', sizeof message);

            while (1)
            {
                fd_max = STDIN_FILENO > client_fd ? STDIN_FILENO : client_fd;

                FD_ZERO(&readFds);
                FD_SET(client->socket_fd, &readFds);
                FD_SET(STDIN_FILENO, &readFds);

                struct timeval timer;
                timer.tv_sec = 2;
                timer.tv_usec = 0;

                int selection = select(fd_max + 1, &readFds, NULL, NULL, &timer);

                if (selection == -1)
                {
                    fprintf(stderr, "Monitor error: %s", strerror(selection));
                    break;
                }

                if (FD_ISSET(client->socket_fd, &readFds))
                {
                    char message[BUFSIZ];

                    char buffer[BUFSIZ];
                    int bytes = recv(client->socket_fd, buffer, sizeof(buffer), 0);

                    if (bytes <= 0)
                    {
                        printf("Server disconnected\n");
                        break;
                    }

                    buffer[bytes] = '\0';
                    printf("Client%s", buffer);
                    printf(": ");
                    fflush(stdout);
                }

                if (FD_ISSET(STDIN_FILENO, &readFds))
                {
                    char message[BUFSIZ];
                    fgets(message, BUFSIZ, stdin);

                    sendData(client, message);
                }
            }
        }
    }

    freeaddrinfo(res);
    return 0;
}