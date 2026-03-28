#include "../../../Header.h"

int createServer(int port);
void acceptConnection(int server_fd, struct pollfd **poll_fds, int *poll_count, int *poll_size);
void readData(int client_fd, struct pollfd **poll_fds, int *poll_count, int *poll_size, int server_fd);
void add_to_poll_fds(int service_fd, struct pollfd **poll_fds, int *poll_count, int *poll_size);
void del_from_poll_fds(int service_fd, struct pollfd **poll_fds, int *poll_count, int *poll_size);

int main()
{
    int server_fd = createServer(PORT);

    switch (server_fd)
    {
    case -1:
        fprintf(stderr, "Socket error: %s\n", strerror(errno));
        return -1;
    case -2:
        fprintf(stderr, "Binding error: %s\n", strerror(errno));
        return -1;
    case -3:
        fprintf(stderr, "Listening error: %s\n", strerror(errno));
        return -1;
    default:
        printf("Server is listening at port %d\n", PORT);
        break;
    }

    int poll_count = 0, poll_size = 10, poll_status;
    struct pollfd *fds = calloc(poll_size + 1, sizeof *fds);

    if (!fds)
    {
        perror("Insufficient memory");
        return -1;
    }

    add_to_poll_fds(server_fd, &fds, &poll_count, &poll_size);

    while (1)
    {
        poll_status = poll(fds, poll_count, 2000);
        if (poll_status == -1)
        {
            fprintf(stderr, "Poll error: %s", strerror(poll_status));
            return -1;
        }
        else if (poll_status == 0)
        {
            printf("Server waiting...\n");
            continue;
        }

        for (int i = 0; i < poll_count; i++)
        {
            if (((fds + i)->revents & POLLIN) != 1)
                continue;

            printf("Client [%d] ready for I/O connection\n", (fds + i)->fd);

            if ((fds + i)->fd == server_fd)
                acceptConnection(server_fd, &fds, &poll_count, &poll_size);
            else
                readData(fds[i].fd, &fds, &poll_count, &poll_size, server_fd);
        }
    }

    return 0;
}

int createServer(int port)
{
    sockaddr_in sockAddrInfo = {
        .sin_addr.s_addr = htonl(INADDR_LOOPBACK),
        .sin_port = htons(port),
        .sin_family = AF_INET};

    int server_fd = socket(sockAddrInfo.sin_family, SOCK_STREAM, 0);

    if (server_fd == -1)
        return -1;

    int binding = bind(server_fd, (const struct sockaddr *)&sockAddrInfo, sizeof sockAddrInfo);
    if (binding != 0)
        return -2;

    int listening = listen(server_fd, BACKLOG);
    if (listening == -1)
        return -3;

    return server_fd;
}

void acceptConnection(int server_fd, struct pollfd **poll_fds, int *poll_count, int *poll_size)
{
    int client_fd = accept(server_fd, NULL, NULL);

    char message[BUFSIZ];
    memset(&message, '\0', sizeof message);

    if (client_fd == -1)
    {
        fprintf(stderr, "Client connection error: %s", strerror(errno));
        return;
    }

    add_to_poll_fds(client_fd, poll_fds, poll_count, poll_size);

    printf("[Server], new client: %d\n", client_fd);
    sprintf(message, "Client connected: [%d]\n", client_fd);

    int sendMessage = send(client_fd, message, strlen(message), 0);

    if (sendMessage == -1)
    {
        fprintf(stderr, "Send message: %s", strerror(errno));
        return;
    }
}

void readData(int client_fd, struct pollfd **poll_fds, int *poll_count, int *poll_size, int server_fd)
{
    char buffer[BUFSIZ], message[BUFSIZ];
    memset(&buffer, '\0', sizeof buffer);
    memset(&message, '\0', sizeof message);

    int bytes_sent = recv(client_fd, buffer, BUFSIZ, 0);

    if (bytes_sent <= 0)
    {
        if (bytes_sent == 0)
            printf("Client connection closed\n");

        if (bytes_sent == -1)
        {
            fprintf(stderr, "Client error: %s", strerror(errno));
            return;
        }

        close(client_fd);
        del_from_poll_fds(client_fd, poll_fds, poll_count, poll_size);
        return;
    }

    printf("Message received: \"%s\"", buffer);
    for (int i = 0; i < *poll_count; i++)
    {
        sprintf(message, "[%d] says: %s", client_fd, buffer);

        int destination_fd = (*poll_fds)[i].fd;

        if (destination_fd != client_fd && destination_fd != server_fd)
        {
            int status = send(destination_fd, message, strlen(message), 0);

            if (status == -1)
            {
                fprintf(stderr, "Data send error: %s\n", strerror(errno));
                return;
            }
        }
    }
}

void add_to_poll_fds(int service_fd, struct pollfd **poll_fds, int *poll_count, int *poll_size)
{
    if (*poll_count >= *poll_size)
    {
        *poll_size *= 2;

        struct pollfd *newPoll_fds = realloc(poll_fds, sizeof(struct pollfd) * (*poll_size));

        if (!newPoll_fds)
        {
            perror("Insufficient memory");
            return;
        }

        *poll_fds = newPoll_fds;
    }

    (*poll_fds)[*poll_count].fd = service_fd;
    (*poll_fds)[*poll_count].events = POLLIN;
    (*poll_count) += 1;
}

void del_from_poll_fds(int service_fd, struct pollfd **poll_fds, int *poll_count, int *poll_size)
{
    for (int i = 0; i < *poll_count; i++)
    {
        if ((*poll_fds)[i].fd == service_fd)
        {
            (*poll_fds)[service_fd] = (*poll_fds)[*poll_count - 1];
            (*poll_count) -= 1;
            break;
        }
    }
}