#include "../../Header.h"

typedef struct
{
    size_t responseLength;
    char *fullData;
    int statusCode;
    int contentLength;
    char *headers;
    char *body;
} Response;

int createServer(int port)
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd == -1)
    {
        fprintf(stderr, "Socket error: %s", strerror(errno));
        return -1;
    }

    return server_fd;
}
addrinfo *addressInfo(char *url)
{
    if (strlen(url) <= 0)
    {
        printf("No request url provided");
        return NULL;
    }

    addrinfo hints, *res;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int getIpAddr = getaddrinfo(url, "80", &hints, &res);

    if (getIpAddr != 0)
    {
        fprintf(stderr, "Address resolution error: %s", gai_strerror(getIpAddr));
        return NULL;
    }

    return res;
}

void GET(int server_fd, const char *url, const char *path);
void POST(int server_fd, const char *url, const char *path, char *body);
void PATCH(int server_fd, const char *url, const char *path, char *body);
void PUT(int server_fd, const char *url, const char *path, char *body);
void DELETE(int server_fd, const char *url, const char *path);
Response *createResponse();
void formatter(Response *httpResponse);
void freeResponseMemory(Response *response);

int main(int argc, const char *args[])
{
    if (argc < 4)
    {
        printf("<url>, <request method>, <path> required");
        return -1;
    }

    char requestUrl[BUFSIZ],
        requestPath[BUFSIZ],
        requestMethod[BUFSIZ];

    snprintf(requestUrl, BUFSIZ, "%s", args[1]);
    snprintf(requestMethod, BUFSIZ, "%s", args[2]);
    snprintf(requestPath, BUFSIZ, "%s", args[3]);

    int server = createServer(PORT);

    if (server == -1)
        return -1;

    addrinfo *ipInfo = addressInfo(requestUrl);

    if (ipInfo == NULL)
        return -2;

    int requestConnection = connect(server, ipInfo->ai_addr, ipInfo->ai_addrlen);
    if (requestConnection == -1)
    {
        fprintf(stderr, "Connection error: %s", strerror(errno));
        return -3;
    }

    switch (strlen(requestMethod))
    {
    case 3:
        for (char *p = requestMethod; *p != '\0'; p++)
            *p = tolower(*p);

        if (strcmp(requestMethod, "get") == 0)
            GET(server, requestUrl, requestPath);
        else
        {
            char *body = "{\"title\":\"New title\",\"body\":\"Test body\",\"id\":\"1\",\"userId\":\"1\"}";
            PUT(server, requestUrl, requestPath, body);
        }
        break;
    case 4:
    {
        char *body = "{\"title\":\"foo\",\"body\":\"bar\",\"userid\":\"1\"}";

        POST(server, requestUrl, requestPath, body);
        break;
    }
    case 5:
    {
        char *body = "{\"title\":\"foobody\"}";

        PATCH(server, requestUrl, requestPath, body);
        break;
    }
    case 6:
        DELETE(server, requestUrl, requestPath);
        break;
    default:
        printf("Unknown request method");
        break;
    }

    return 0;
}

void GET(int server_fd, const char *url, const char *path)
{
    char message[BUFSIZ],
        buffer[BUFSIZ];

    Response *getResponse = createResponse();

    snprintf(message, BUFSIZ,
             "GET %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "User-Agent: Muchiri/1.0\r\n"
             "Connection: close\r\n"
             "\r\n",
             path, url);

    int sendRequest = send(server_fd, message, strlen(message), 0),
        bytes_sent;

    size_t totalReceived = 0;

    while ((bytes_sent = recv(server_fd, buffer, BUFSIZ - 1, 0)) > 0)
    {
        memcpy(getResponse->fullData + totalReceived, buffer, bytes_sent);
        totalReceived += bytes_sent;
        getResponse->responseLength += totalReceived;
        getResponse->fullData[totalReceived] = '\0';
    }

    if (bytes_sent < 0)
    {
        fprintf(stderr, "Receiving error: %s", strerror(errno));
        return;
    }

    formatter(getResponse);
    printf("-----FORMATTED RESPONSE----\n\n");
    printf("Response status code: %d\n", getResponse->statusCode);
    printf("Response length: %d\n\n", getResponse->contentLength);
    printf("Response headers: %s\n\n", getResponse->headers);
    printf("Response body:%s\n\n", getResponse->body);

    freeResponseMemory(getResponse);
}
void POST(int server_fd, const char *url, const char *path, char *body)
{
    char message[BUFSIZ],
        buffer[BUFSIZ];

    size_t bodyLength = strlen(body);

    snprintf(message, BUFSIZ,
             "POST %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "User-Agent: Muchiri/1.0\r\n"
             "Content-type: application/json\r\n"
             "Content-length: %zu\r\n"
             "Connection: close\r\n"
             "\r\n"
             "%s",
             path, url, bodyLength, body);

    int postRequest = send(server_fd, message, strlen(message), 0),
        bytes_sent,
        totalBytes = 0;

    Response *postResponse = createResponse();

    if (postRequest == -1)
    {
        fprintf(stderr, "Request error: %s", strerror(postRequest));
        return;
    }

    while ((bytes_sent = recv(server_fd, buffer, BUFSIZ - 1, 0)) > 0)
    {
        memcpy(postResponse->fullData + totalBytes, buffer, bytes_sent);
        postResponse->responseLength += totalBytes;
        totalBytes += bytes_sent;
        postResponse->fullData[totalBytes] = '\0';
    }

    if (bytes_sent < 0)
    {
        fprintf(stderr, "Response error: %s", strerror(bytes_sent));
        return;
    }

    formatter(postResponse);
    printf("-------FORMATTED RESPONSE-------\n\n");
    printf("Response status: %d\n", postResponse->statusCode);
    printf("Response length: %d\n\n", postResponse->contentLength);
    printf("Response headers: %s\n\n", postResponse->headers);
    printf("Response body: %s\n\n", postResponse->body);

    freeResponseMemory(postResponse);
}
void PATCH(int server_fd, const char *url, const char *path, char *body)
{
    char requestMsg[BUFSIZ],
        buffer[BUFSIZ] = {0};

    size_t bodyLength = strlen(body);

    snprintf(requestMsg, BUFSIZ,
             "PATCH %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "User-Agent: Muchiri/1.0\r\n"
             "Content-type: application/json\r\n"
             "Content-length: %zu\r\n"
             "Connection: close\r\n"
             "\r\n"
             "%s",
             path, url, bodyLength, body);

    int putRequest = send(server_fd, requestMsg, strlen(requestMsg), 0),
        bytesSent, fullResponseLength = 0,
        totalSize = 0;

    Response *patchResponse = createResponse();

    if (putRequest < 0)
    {
        fprintf(stderr, "Send error: %s", strerror(errno));
        return;
    }

    while ((bytesSent = recv(server_fd, buffer, BUFSIZ - 1, 0)) > 0)
    {
        memcpy(patchResponse->fullData + totalSize, buffer, bytesSent);
        totalSize += bytesSent;
        patchResponse->responseLength += totalSize;
        patchResponse->fullData[totalSize] = '\0';
    }

    if (bytesSent == -1)
    {
        fprintf(stderr, "Receiving error: %s", strerror(errno));
        return;
    }

    formatter(patchResponse);
    printf("-------FORMATTED RESPONSE-------\n\n");
    printf("Response status: %d\n", patchResponse->statusCode);
    printf("Response length: %d\n\n", patchResponse->contentLength);
    printf("Response headers: %s\n\n", patchResponse->headers);
    printf("Response body: %s\n\n", patchResponse->body);

    freeResponseMemory(patchResponse);
}
void PUT(int server_fd, const char *url, const char *path, char *body)
{
    char buffer[BUFSIZ] = {0},
         requestHeaders[BUFSIZ];

    size_t bodyLength = strlen(body);

    snprintf(requestHeaders, BUFSIZ,
             "PUT %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "User-Agent: Muchiri/1.0\r\n"
             "Content-type: application/json\r\n"
             "Content-length: %zu\r\n"
             "Connection: close\r\n"
             "\r\n"
             "%s",
             path, url, bodyLength, body);

    int putRequest = send(server_fd, requestHeaders, strlen(requestHeaders), 0),
        bytes_sent, totalSize = 0;

    Response *putResponse = createResponse();

    if (putRequest < 0)
    {
        fprintf(stderr, "Send error: %s", strerror(errno));
        return;
    }

    while ((bytes_sent = recv(server_fd, buffer, BUFSIZ - 1, 0)) > 0)
    {
        memcpy(putResponse->fullData + totalSize, buffer, bytes_sent);
        totalSize += bytes_sent;
        putResponse->responseLength += totalSize;
        putResponse->fullData[totalSize] = '\0';
    }

    if (bytes_sent < 0)
    {
        fprintf(stderr, "Receiving error: %s", strerror(errno));
        return;
    }

    formatter(putResponse);
    printf("-------FORMATTED RESPONSE-------\n\n");
    printf("Response status: %d\n", putResponse->statusCode);
    printf("Response length: %d\n\n", putResponse->contentLength);
    printf("Response headers: %s\n\n", putResponse->headers);
    printf("Response body: %s\n\n", putResponse->body);

    freeResponseMemory(putResponse);
}
void DELETE(int server_fd, const char *url, const char *path)
{
    char buffer[BUFSIZ],
        requestHeaders[BUFSIZ];

    snprintf(requestHeaders, BUFSIZ,
             "DELETE %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "User-Agent: Muchiri/1.0\r\n"
             "Connection: close\r\n"
             "\r\n",
             path, url);

    int deleteRequest = send(server_fd, requestHeaders, strlen(requestHeaders), 0), bytes_sent, totalSize = 0;
    Response *deleteResponse = createResponse();

    if (deleteRequest < 0)
    {
        fprintf(stderr, "Sending error: %s", strerror(errno));
        return;
    }

    while ((bytes_sent = recv(server_fd, buffer, BUFSIZ - 1, 0) > 0))
    {
        memcpy(deleteResponse->fullData + totalSize, buffer, bytes_sent);
        totalSize += bytes_sent;
        deleteResponse->responseLength += totalSize;
        deleteResponse->fullData[totalSize] = '\0';
    }

    if (bytes_sent == -1)
    {
        fprintf(stderr, "Receiving error: %s", strerror(errno));
        return;
    }

    formatter(deleteResponse);
    printf("-------FORMATTED RESPONSE-------\n\n");
    printf("Response status: %d\n", deleteResponse->statusCode);
    printf("Response length: %d\n\n", deleteResponse->contentLength);
    printf("Response headers: %s\n\n", deleteResponse->headers);
    printf("Response body: %s\n\n", deleteResponse->body);

    freeResponseMemory(deleteResponse);
}
Response *createResponse()
{
    Response *newResponse = malloc(sizeof(Response));

    if (!newResponse)
    {
        perror("Memory");
        return NULL;
    }

    newResponse->body = malloc(BUFSIZ);
    newResponse->fullData = malloc(BUFSIZ);
    newResponse->responseLength = 0;
    newResponse->headers = malloc(BUFSIZ);
    newResponse->statusCode = 0;
    newResponse->contentLength = 0;

    return newResponse;
}
void formatter(Response *httpResponse)
{
    if (!httpResponse)
    {
        perror("Invalid response");
        return;
    }

    char *headers = strstr(httpResponse->fullData, "\r\n\r\n"),
         *contentLength = strstr(httpResponse->fullData, "Content-Length");

    if (headers)
    {
        *headers = '\0';
        httpResponse->headers = httpResponse->fullData;

        httpResponse->body = headers + 4;

        sscanf(httpResponse->headers, "HTTP/1.1 %d", &httpResponse->statusCode);
    }
    if (contentLength)
        sscanf(httpResponse->headers, "Content-Length: %d", &httpResponse->statusCode);
}
void freeResponseMemory(Response *response)
{
    if (!response)
    {
        perror("Memory fault");
        return;
    }

    free(response->fullData);
    free(response);
}
