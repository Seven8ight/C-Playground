#include "../../Header.h"

int main(int argc, char *argv[])
{
    printf("--------SERVER--------\n\n");

    sockaddr_in sa;
    int server_fd, client_fd, status;
    sockaddr_storage client_addr;
    socklen_t addr_size;
    char buffer[BUFSIZ];
    int bytes_read;

    __uint16_t serverPort = 4030;

    memset(&sa, 0, sizeof sa);

    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    sa.sin_port = htons(serverPort);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        fprintf(stderr, "Socket creation error: %s\n", strerror(errno));
        return 1;
    }

    fcntl(server_fd, F_SETFL, O_NONBLOCK); // makes the server fd non blocking towards socket and accept function calls.
    printf("Created server socket fd: %d\n", server_fd);

    status = bind(server_fd, (struct sockaddr *)&sa, sizeof sa);
    if (status != 0)
    {
        fprintf(stderr, "Bind error: %s\n", strerror(errno));
        return 2;
    }

    printf("Server binded\n");

    status = listen(server_fd, BACKLOG);
    if (status != 0)
    {
        fprintf(stderr, "Listening error: %s\n", strerror(errno));
        return 3;
    }

    addr_size = sizeof client_addr;
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_size);
    if (client_fd == -1)
    {
        fprintf(stderr, "Client fd error: %s\n", strerror(errno));
        return 4;
    }

    printf("Accepted new Connection on client socket fd: %d\n", client_fd);

    bytes_read = 1;

    while (bytes_read >= 0)
    {
        printf("Reading client socket: %d\n", client_fd);
        bytes_read = recv(client_fd, buffer, BUFSIZ, 0);

        if (bytes_read == 0)
        {
            printf("Client socket %d: closed connection.\n", client_fd);
            break;
        }
        else if (bytes_read == -1)
        {
            fprintf(stderr, "Recv error: %s\n", strerror(errno));
            break;
        }
        else
        {
            char *msg = "Got your message";
            int msg_len = strlen(msg), bytes_sent;

            buffer[bytes_read] = '\0';
            printf("Message received from client socket %d: \"%s\" \n", client_fd, buffer);

            bytes_sent = send(client_fd, msg, msg_len, 0);
            if (bytes_sent == -1)
                fprintf(stderr, "Send error: %s\n", strerror(errno));
            else if (bytes_sent == msg_len)
                printf("Sent full message to client socket %d: \"%s\"\n", client_fd, msg);
            else
                printf("Sent partial message to client socket: %d: %d bytes sent.\n", client_fd, bytes_sent);
        }
    }

    printf("Closing client socket\n");
    close(client_fd);
    printf("Closing server socket\n");
    close(server_fd);

    return 0;
}