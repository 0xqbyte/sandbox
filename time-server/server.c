#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>

#define PORT 8080

int main()
{
    int socket_fd;
    int opt = 1;
    int err;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        perror("socket");
        return 1;
    }

    err = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (err < 0)
    {
        perror("setsockopt");
        return 1;
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    err = bind(socket_fd, (struct sockaddr *)&address, sizeof(address));
    if (err < 0)
    {
        perror("bind");
        return 1;
    }

    err = listen(socket_fd, 3);
    if (err < 0)
    {
        perror("listen");
        return 1;
    }

    while (true)
    {
        socklen_t addrlen = sizeof(address);
        int conn_fd = accept(socket_fd, (struct sockaddr *)&address, &addrlen);
        if (conn_fd < 0)
        {
            perror("accept");
            return 1;
        }

        time_t rawtime;
        struct tm *timeinfo;

        time(&rawtime);
        timeinfo = localtime(&rawtime);
        char *message = asctime(timeinfo);

        err = send(conn_fd, message, strlen(message), 0);
        if (err < 0)
        {
            perror("send");
            return 1;
        }

        close(conn_fd);
    }

    close(socket_fd);

    return 0;
}