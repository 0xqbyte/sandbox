#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>

#define PORT 8080
#define QUOTES_LEN 1000

static char *read_file(const char *path);
static int parse_quotes(char *content, char *quotes[]);
static char *get_random_quote(char **quotes, int n);

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: qotd filename\n");
        return 1;
    }

    srand(time(NULL));

    char *filename = argv[1];
    char *content = read_file(filename);
    if (content == NULL)
    {
        perror("read_file");
        return 1;
    }

    char *quotes[QUOTES_LEN] = {0};
    int n = parse_quotes(content, quotes);

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

        char *quote = get_random_quote(quotes, n);
        char message[strlen(quote) + 2];
        sprintf(message, "\n%s\n", quote);
        err = send(conn_fd, message, strlen(message), 0);
        if (err < 0)
        {
            perror("send");
            return 1;
        }

        close(conn_fd);
    }

    free(content);

    close(socket_fd);
}

static char *get_random_quote(char **quotes, int n)
{
    return quotes[rand() % n];
}

static int parse_quotes(char *content, char *quotes[])
{
    const char *delimeter = "%";
    char *quote;
    int n = 0;

    quote = strtok(content, delimeter);
    while (quote != NULL)
    {
        if (*quote == '\n')
            quote++;
        quotes[n++] = quote;
        quote = strtok(NULL, delimeter);
    }

    return n;
}

static char *read_file(const char *path)
{
    struct stat f_stat;
    if (stat(path, &f_stat) < 0)
        return NULL;

    FILE *file = fopen(path, "r");
    if (file == NULL)
        return NULL;

    char *content = malloc(f_stat.st_size + 1);
    if (content == NULL)
        return NULL;

    char *p = content;
    int c;
    while ((c = fgetc(file)) != EOF)
        *p++ = c;
    *p = '\0';

    fclose(file);

    return content;
}