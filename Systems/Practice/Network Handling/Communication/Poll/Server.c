#include "../../Header.h"

int createServer(const int port)
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
        return -1;

    sockaddr_in socketAInfo;
    memset(&socketAInfo, 0, sizeof socketAInfo);

    socketAInfo.sin_family = AF_INET;
    socketAInfo.sin_port = htons(port);
    socketAInfo.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    socklen_t socketLength = sizeof socketAInfo;

    int bindSocket = bind(socket_fd, (const struct sockaddr *)&socketAInfo, socketLength);
    if (bindSocket == -1)
        return -1;

    int listening = listen(socket_fd, BACKLOG);
    if (listening == -1)
        return -1;

    return socket_fd;
}

void add_to_poll_fds(struct pollfd *poll_fds[], int new_fd, int *poll_count, int *poll_size)
{
    if (*poll_count == *poll_size)
    {
        *poll_size *= 2;
        *poll_fds = realloc(*poll_fds, sizeof **poll_fds * (*poll_size));
    }

    (*poll_fds)[*poll_count].fd = new_fd;
    (*poll_fds)[*poll_count].events = POLLIN;
    (*poll_count)++;
}

void del_from_poll_fds(struct pollfd **poll_fds, int i, int *poll_count)
{
    (*poll_fds)[i] = (*poll_fds)[*poll_count - 1];
    (*poll_count)--;
}

void acceptConnections(int server_fd, struct pollfd **poll_fds, int *poll_count, int *poll_size)
{
    int client_fd, status;
    char message[BUFSIZ];

    client_fd = accept(server_fd, NULL, NULL);
    if (client_fd == -1)
    {
        fprintf(stderr, "[Server] Accept error: %s\n", strerror(errno));
        return;
    }

    add_to_poll_fds(poll_fds, client_fd, poll_count, poll_size);

    printf("[Server] accepted new connection on client socket %d\n", client_fd);

    memset(&message, '\0', sizeof message);
    sprintf(message, "Welcome. You are client fd [%d]\n", client_fd);
    status = send(client_fd, message, strlen(message), 0);

    if (status == -1)
        fprintf(stderr, "[Server] send error to client %d: %s\n", client_fd, strerror(errno));
}

void read_data_from_socket(int i, struct pollfd **poll_fds, int *poll_count, int server_fd)
{
    char buffer[BUFSIZ],
        message[BUFSIZ];

    int bytes_read, status, dest_fd, sender_fd;

    sender_fd = (*poll_fds)[i].fd;
    memset(&buffer, '\0', sizeof buffer);
    bytes_read = recv(sender_fd, buffer, BUFSIZ, 0);

    if (bytes_read <= 0)
    {
        if (bytes_read == 0)
            printf("[%d]Client socket closed connection\n", sender_fd);
        else
            fprintf(stderr, "[Server] Recv error: %s\n", strerror(errno));

        close(sender_fd);
        del_from_poll_fds(poll_fds, i, poll_count);
    }
    else
    {
        printf("[%d] Got message: %s", sender_fd, buffer);
        memset(&message, '\0', sizeof message);
        sprintf(buffer, "[%d] says: %s", sender_fd, buffer);

        for (int j = 0; j < *poll_count; j++)
        {
            dest_fd = (*poll_fds)[j].fd;
            if (dest_fd != server_fd && dest_fd != sender_fd)
            {
                status = send(dest_fd, message, strlen(message), 0);

                if (status == -1)
                    fprintf(stderr, "[Server] Send error to client fd %d: %s\n", dest_fd, strerror(errno));
            }
        }
    }
}

int main()
{
    int server_fd = createServer(PORT), status;
    if (server_fd == -1)
    {
        fprintf(stderr, "Server creation error: %s", strerror(errno));
        return -1;
    }

    struct pollfd *poll_fds;
    int poll_size, poll_count;

    poll_size = 5;
    poll_fds = calloc(poll_size + 1, sizeof *poll_fds);
    if (!poll_fds)
        return 4;

    poll_fds[0].fd = server_fd;
    poll_fds[0].events = POLLIN;
    poll_count = 1;

    printf("[Server] Set up poll fd array\n");

    while (1)
    { // Main loop
        // Poll sockets to see if they are ready (2 second timeout)
        status = poll(poll_fds, poll_count, 2000);
        if (status == -1)
        {
            fprintf(stderr, "[Server] Poll error: %s\n", strerror(errno));
            return -1;
        }
        else if (status == 0)
        {
            // None of the sockets are ready
            printf("[Server] Waiting...\n");
            continue;
        }

        // Loop on our array of sockets
        for (int i = 0; i < poll_count; i++)
        {
            if ((poll_fds[i].revents & POLLIN) != 1)
            {
                // The socket is not ready for reading
                // stop here and continue the loop
                continue;
            }
            printf("[%d] Ready for I/O operation\n", poll_fds[i].fd);
            // The socket is ready for reading!
            if (poll_fds[i].fd == server_fd)
            {
                // Socket is our listening server socket
                acceptConnections(server_fd, &poll_fds, &poll_count, &poll_size);
            }
            else
            {
                // Socket is a client socket, read it
                read_data_from_socket(i, &poll_fds, &poll_count, server_fd);
            }
        }
    }
    return (0);
}