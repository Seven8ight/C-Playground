#include "Header.h"

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

    fread(fileContents, fileSize, sizeof(char), file);
    fileContents[fileSize] = '\0';

    fclose(file);
    return fileContents;
}

void acceptConnections(Server *server, fd_set *allFds, int *fd_max)
{
    if (!server || !allFds)
    {
        perror('Memory');
        return;
    }

    int client_fd = accept(server->fd, server->serverInfo->ai_addr, server->serverInfo->ai_addrlen);
    char requestBuffer[BUFSIZ] = {0},
         fullRequest[BUFSIZ] = {0};

    if (client_fd == -1)
    {
        fprintf(stderr, "Connection error: %s", strerror(errno));
        return;
    }

    FD_SET(client_fd, allFds);

    for (int i = 0; i < *fd_max; i++)
    {
        if (FD_ISSET(i, allFds))
        {
            if (i > *fd_max)
                *fd_max = i;

            int bytes_sent, totalSize = 0;

            while ((bytes_sent = recv(client_fd, requestBuffer, BUFSIZ, 0) > 0))
            {
                memcpy(fullRequest + totalSize, requestBuffer, bytes_sent);
                totalSize += bytes_sent;
            }
        }
    }
}