/* Simple blocking echo server */

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define RECV_BUFF_SIZE 1024

static void handle_conn(int conn_fd, struct sockaddr_in addr);

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

    err = listen(socket_fd, SOMAXCONN);
    if (err < 0)
    {
        perror("listen");
        return 1;
    }

    printf("server is listening on %d\n", PORT);

    while (true)
    {
        struct sockaddr_in conn_address;
        socklen_t conn_address_len = sizeof(conn_address);
        int conn_fd = accept(socket_fd, (struct sockaddr *)&conn_address, &conn_address_len);
        if (conn_fd < 0)
        {
            perror("accept");
            return 1;
        }

        /* todo: concurrent handler */
        handle_conn(conn_fd, conn_address);
    }

    close(socket_fd);

    return 0;
}

static void handle_conn(int conn_fd, struct sockaddr_in addr)
{
    char ip[INET_ADDRSTRLEN];
    uint16_t port;
    char buff[RECV_BUFF_SIZE];
    ssize_t recv_n, send_n;

    inet_ntop(AF_INET, &addr.sin_addr, ip, sizeof(ip));
    port = ntohs(addr.sin_port);

    printf("%s:%d connected\n", ip, port);

    while (true)
    {
        recv_n = recv(conn_fd, buff, RECV_BUFF_SIZE, 0);
        if (recv_n <= 0)
        {
            perror("recv");
            close(conn_fd);
            return;
        }

        send_n = send(conn_fd, buff, recv_n, 0);
        if (send_n <= 0)
        {
            perror("send");
            close(conn_fd);
            return;
        }
    }
}