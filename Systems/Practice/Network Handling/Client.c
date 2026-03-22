#include "Header.h"

int main(int args, char *argv[])
{
    printf("--------CLIENT--------\n\n");

    sockaddr_in sa;
    int socket_fd, status, bytes_read, msg_len, bytes_sent;
    char buffer[BUFSIZ], *msg;

    if (args != 2)
    {
        printf("Usage: /.client \"Message to send\"");
        return 1;
    }

    __uint16_t serverPort = 4030;

    memset(&sa, 0, sizeof sa);
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    sa.sin_port = htons(serverPort);

    socket_fd = socket(sa.sin_family, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        fprintf(stderr, "Socket fd error: %s\n", strerror(errno));
        return 2;
    }

    printf("Created socket fd: %d\n", socket_fd);

    status = connect(socket_fd, (struct sockaddr *)&sa, sizeof sa);
    if (status != 0)
    {
        fprintf(stderr, "Connect error: %s\n", strerror(errno));
        return 3;
    }

    msg = argv[1];
    msg_len = strlen(msg);
    bytes_sent = send(socket_fd, msg, msg_len, 0);
    if (bytes_sent == -1)
        fprintf(stderr, "Send error: %s\n", strerror(errno));
    else if (bytes_sent == msg_len)
        printf("Message successfully sent\n");
    else
        printf("Data loss, sent message partially: %d bytes sent\n", bytes_sent);

    bytes_read = 1;
    while (bytes_read >= 0)
    {
        bytes_read = recv(socket_fd, buffer, BUFSIZ, 0);
        if (bytes_read == 0)
        {
            printf("Server closed connection.\n");
            break;
        }
        else if (bytes_read == -1)
        {
            fprintf(stderr, "Recv error: %s\n", strerror(errno));
            break;
        }
        else
        {
            buffer[bytes_read] = '\0';
            printf("Message received: \"%s\"\n", buffer);
            break;
        }
    }

    printf("Closing socket");
    close(socket_fd);

    return 0;
}