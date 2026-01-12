/* Simple echo server, process per each conn */

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT 8080
#define RECV_BUFF_SIZE 1024

static void handle_conn(int conn_fd, struct sockaddr_in addr);

/*
reap - is handler for waiting child processed
to allow child zombie processes to be removed
*/
static void reap(int sig);

int main()
{
    int socket_fd;
    int opt = 1;
    int err;

    /*
    When a child process stops or terminates, SIGCHLD is sent to the parent process,
    reap - is handler for waiting child processed to allow child zombie processes
    to be removed
    */
    signal(SIGCHLD, reap);

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

        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork");
            return 1;
        }

        if (pid == 0) /* child process */
        {
            /*
            child process must close unused server socket file descriptor
            because child doesn't accept new connections
            */
            close(socket_fd);

            handle_conn(conn_fd, conn_address);

            _exit(0);
        }

        printf("pid=%d\n", pid);

        /* parent process must close unused conn file descriptor */
        close(conn_fd);
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
            if (recv_n == 0)
            {
                printf("%s:%d closed\n", ip, port);
                close(conn_fd);
                return;
            }

            perror("recv");
            close(conn_fd);
            return;
        }

        /*
        send() doesn't guarantee sending all bytes and
        can silently drop data under pressure, for that
        it's better to partial write
        */
        ssize_t total = 0;
        while (total < recv_n)
        {
            send_n = send(conn_fd, buff + total, recv_n - total, 0);
            if (send_n <= 0)
            {
                perror("send");
                close(conn_fd);
                return;
            }
            total += send_n;
        }
    }
}

static void reap(int sig)
{
    (void)sig;
    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;
}