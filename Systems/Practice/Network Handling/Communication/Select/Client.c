#include "../../Header.h"

int main(int argc, const char *args[])
{
    if (argc == 0)
    {
        printf("Provide a message alongside execution");
        return -1;
    }

    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1)
    {
        fprintf(stderr, "%s", strerror(errno));
        return 1;
    }

    sockaddr_in clientAInfo;
    clientAInfo.sin_family = AF_INET;
    clientAInfo.sin_port = htons(PORT);
    clientAInfo.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    socklen_t clientSize = sizeof clientAInfo;

    int serverConnect = connect(client_fd, (struct sockaddr *)&clientAInfo, clientSize);

    if (serverConnect == -1)
    {
        fprintf(stderr, "Client connect error: %s", strerror(errno));
        return 2;
    }

    char buffer[BUFSIZ],
        *message;

    message = (char *)args[1];

    send(client_fd, message, strlen(message), 0);

    int receiveData = 1;

    while (receiveData >= 0)
    {
        receiveData = recv(client_fd, buffer, BUFSIZ, 0);

        if (receiveData == 0)
        {
            printf("No data received, server closed connection\n");
            break;
        }
        else if (receiveData == -1)
        {
            fprintf(stderr, "%s", strerror(errno));
        }
        else
        {
            buffer[receiveData] = '\0';
            printf("Message received: \"%s\"\n", buffer);
            break;
        }
    }

    printf("Closing socket");
    close(client_fd);

    return 0;
}