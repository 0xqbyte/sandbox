/* Simple echo server, thread per each conn */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "channel.h"
#include "queue.h"

#define PORT 8080
#define RECV_BUFF_SIZE 1024
#define POOL_SIZE 8

static pthread_t pool[POOL_SIZE];
static Channel *chan;

struct conn
{
    int fd;
    struct sockaddr_in addr;
};

static void handle_conn(struct conn *c);
static void *handler(void *arg);

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

    chan = channel_init();
    if (chan == NULL)
    {
        perror("channel_init");
        return 1;
    }

    printf("init pool: size=%d\n", POOL_SIZE);

    for (int i = 0; i < POOL_SIZE; i++)
    {
        err = pthread_create(&pool[i], NULL, handler, NULL);
        if (err != 0)
        {
            perror("pthread_create");
            continue;
        }
    }

    printf("server is listening in %d\n", PORT);

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

        struct conn *c = malloc(sizeof(struct conn));
        if (c == NULL)
        {
            perror("malloc");
            return 1;
        }

        c->fd = conn_fd;
        c->addr = conn_address;

        channel_send(chan, c);
    }

    channel_free(chan);

    close(socket_fd);

    return 0;
}

static void *handler(void *arg)
{
    (void)arg;

    struct conn *c;
    while ((c = channel_recv(chan)) != NULL)
    {
        handle_conn(c);
        free(c);
    }

    return NULL;
}

static void handle_conn(struct conn *c)
{
    char ip[INET_ADDRSTRLEN];
    uint16_t port;
    char buff[RECV_BUFF_SIZE];
    ssize_t recv_n, send_n;

    inet_ntop(AF_INET, &c->addr.sin_addr, ip, sizeof(ip));
    port = ntohs(c->addr.sin_port);

    printf("%s:%d connected\n", ip, port);

    while (true)
    {
        recv_n = recv(c->fd, buff, RECV_BUFF_SIZE, 0);
        if (recv_n <= 0)
        {
            if (recv_n == 0)
                printf("%s:%d closed\n", ip, port);
            else
                perror("recv");

            close(c->fd);
            return;
        }

        /**
         * send() doesn't guarantee sending all bytes and
         * can silently drop data under pressure, for that
         * it's better to partial write
         */
        ssize_t total = 0;
        while (total < recv_n)
        {
            send_n = send(c->fd, buff + total, recv_n - total, 0);
            if (send_n <= 0)
            {
                perror("send");
                close(c->fd);
                return;
            }
            total += send_n;
        }
    }
}