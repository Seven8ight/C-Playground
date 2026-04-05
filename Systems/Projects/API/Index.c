#include "Header.h"

void SigPipe()
{
    printf("Pipe error\n");
}

Server *createServer(int port)
{
    Server *newServer = malloc(sizeof(Server));
    char serverPort[BUFSIZ] = {0};

    if (!newServer)
    {
        perror("Memory");
        return NULL;
    }

    snprintf(serverPort, BUFSIZ, "%d", port);

    addrinfo hints, *dnsResults;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int getLocalHost = getaddrinfo("127.0.0.1", serverPort, &hints, &dnsResults);

    if (getLocalHost != 0)
    {
        fprintf(stderr, "DNS lookup error: %s\n", gai_strerror(getLocalHost));

        free(newServer);
        return NULL;
    }

    newServer->type = dnsResults->ai_family;
    newServer->serverInfo = dnsResults;

    int server_fd = socket(dnsResults->ai_family, dnsResults->ai_socktype, 0);

    if (server_fd == -1)
    {
        fprintf(stderr, "Socket error: %s\n", strerror(errno));
        free(newServer);
        return NULL;
    }

    newServer->fd = server_fd;

    int binding = bind(newServer->fd, dnsResults->ai_addr, dnsResults->ai_addrlen);

    if (binding < 0)
    {
        fprintf(stderr, "Bind error: %s\n", strerror(errno));
        free(newServer);
        return NULL;
    }

    int listening = listen(server_fd, BACKLOG);

    if (listening < 0)
    {
        fprintf(stderr, "Listening error: %s\n", strerror(errno));
        free(newServer);
        return NULL;
    }

    printf("Server is listening at port %s\n", serverPort);

    return newServer;
}

FILE *fileOpener(char *sourcePath)
{
    if (strlen(sourcePath) <= 0)
    {
        printf("Invalid source path\n");
        return NULL;
    }

    FILE *file = fopen(sourcePath, "r");

    if (!file)
    {
        printf("File is non-existent\n");
        return NULL;
    }

    return file;
}

char *fileContents(FILE *file)
{
    if (!file)
    {
        printf("Invalid file descriptor\n");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    char *fileContents = calloc(fileSize + 1, sizeof(char));
    if (!fileContents)
    {
        perror("Memory");
        return NULL;
    }

    fread(fileContents, sizeof(char), fileSize, file);
    fileContents[fileSize] = '\0';

    return fileContents;
}

void acceptConnections(Server *server, fd_set *allFds, int *fd_max)
{
    if (!server || !allFds)
    {
        perror("Memory");
        return;
    }

    int client_fd = accept(server->fd, NULL, NULL);
    char requestBuffer[BUFSIZ] = {0};

    if (client_fd == -1)
    {
        fprintf(stderr, "Connection error: %s", strerror(errno));
        return;
    }

    FD_SET(client_fd, allFds);

    if (client_fd > *fd_max)
        *fd_max = client_fd;
}

void readSocketData(int clientFd, fd_set *fds, int *fd_max)
{
    char buffer[BUFSIZ];

    int bytes_sent = recv(clientFd, buffer, BUFSIZ - 1, 0);

    if (bytes_sent <= 0)
    {
        if (clientFd == *fd_max)
        {
            int j = 0;
            for (int i = *fd_max; i >= 0; i--)
            {

                if (FD_ISSET(i, fds))
                {
                    if (j < i)
                        j = i;
                }
            }

            *fd_max = j;
        }

        FD_CLR(clientFd, fds);
        return;
    }

    buffer[bytes_sent] = '\0';

    FILE *html = fileOpener("./Pages/index.html");

    if (!html)
    {
        perror("File handling");
        return;
    }

    char *htmlContent = fileContents(html);
    if (!htmlContent)
    {
        fclose(html);
        return;
    }

    size_t htmlContentSize = strlen(htmlContent);

    char responseBody[BUFSIZ * 2];

    snprintf(responseBody, sizeof(responseBody),
             "HTTP/1.1 200 OK\r\n"
             "Content-type: text/html\r\n"
             "Content-length: %zu\r\n"
             "Connection: close\r\n"
             "\r\n"
             "%s",
             htmlContentSize, htmlContent);

    int httpResponse = send(clientFd, responseBody, strlen(responseBody), 0);
    if (httpResponse == -1)
        fprintf(stderr, "Response error: %s\n", strerror(errno));

    if (clientFd == *fd_max)
    {
        int j = 0;
        for (int i = *fd_max; i >= 0; i--)
        {

            if (FD_ISSET(i, fds))
            {
                if (j < i)
                    j = i;
            }
        }

        *fd_max = j;
    }

    FD_CLR(clientFd, fds);
    close(clientFd);
    fclose(html);
}

char *fileExtension(char *requestBuffer)
{
}