#include "../../../Header.h"

int createServer(int port)
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
        return -1;

    sockaddr_in serverInfo;
    memset(&serverInfo, 0, sizeof serverInfo);

    serverInfo.sin_family = AF_INET;
    serverInfo.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    serverInfo.sin_port = htons(PORT);

    socklen_t serverLength = sizeof serverInfo;

    int binding = bind(server_fd, (const struct sockaddr *)&serverInfo, serverLength);
    if (binding == -1)
        return -2;

    int listening = listen(server_fd, BACKLOG);
    if (listening == -1)
        return -3;

    return server_fd;
}

void acceptConnection(int server_fd, fd_set *allSockets, int *fd_max)
{
    int client_fd = accept(server_fd, NULL, NULL),
        status;

    if (client_fd == -1)
    {
        fprintf(stderr, "%s", strerror(errno));
        return;
    }

    char message[BUFSIZ];

    FD_SET(client_fd, allSockets);

    if (client_fd > *fd_max)
        *fd_max = client_fd;

    memset(&message, '\0', sizeof message);
    sprintf(message, "Client %d connected successfully", client_fd);
    status = send(client_fd, message, strlen(message), 0);

    if (status == -1)
    {
        fprintf(stderr, "Client %d error: %s", client_fd, strerror(errno));
        return;
    }
}

void readData(int client_fd, fd_set *allSockets, int *maxFd, int server_fd)
{
    char buffer[BUFSIZ], message[BUFSIZ];
    int bytes_sent = 1, status;

    memset(&buffer, '\0', sizeof buffer);
    bytes_sent = recv(client_fd, buffer, BUFSIZ, 0);

    if (bytes_sent <= 0)
    {
        if (bytes_sent == 0)
        {
            printf("Client connection closed unexpectedly\n");
            return;
        }
        else
            fprintf(stderr, "Client %d error: %s", client_fd, strerror(errno));

        close(client_fd);
        FD_CLR(client_fd, allSockets);
    }
    else
    {
        printf("[%d] got message: %s", client_fd, buffer);

        memset(&message, '\0', sizeof message);
        sprintf(message, "Client [%d] says, hello world", client_fd);

        for (int i = 0; i <= *maxFd; i++)
        {
            if (FD_ISSET(i, allSockets) && i != server_fd && i != client_fd)
            {
                status = send(i, message, strlen(message), 0);

                if (status == -1)
                {
                    fprintf(stderr, "Send error: %s", strerror(errno));
                    return;
                }
            }
        }
    }
}

int main(void)
{
    int server_fd = createServer(PORT);
    switch (server_fd)
    {
    case -1:
        fprintf(stderr, "Socket creation error: %s\n", strerror(errno));
        return -1;
    case -2:
        fprintf(stderr, "Binding error: %s\n", strerror(errno));
        return -1;
    case -3:
        fprintf(stderr, "Listening error: %s\n", strerror(errno));
        return -1;
    default:
        printf("Server listening at port %d\n", PORT);
        break;
    }

    struct timeval timer;
    fd_set allSockets, read_fds;
    int fd_max;

    FD_ZERO(&allSockets);
    FD_ZERO(&read_fds);
    FD_SET(server_fd, &allSockets);
    fd_max = server_fd;

    while (true)
    {
        read_fds = allSockets;

        timer.tv_sec = 2;
        timer.tv_usec = 0;
        int fd_monitor = select(fd_max + 1, &read_fds, NULL, NULL, &timer);

        if (fd_monitor == -1)
        {
            fprintf(stderr, "Select error: %s", strerror(errno));
            break;
        }
        else if (fd_monitor == 0)
        {
            printf("[Server]:Waiting for connection\n");
            continue;
        }

        for (int i = 0; i <= fd_max; i++)
        {
            if (FD_ISSET(i, &read_fds) != 1)
                continue;

            printf("[%d] I/O Connection\n", i);

            if (i == server_fd)
                acceptConnection(server_fd, &allSockets, &fd_max);
            else
                readData(i, &allSockets, &fd_max, server_fd);
        }
    }

    return 0;
}