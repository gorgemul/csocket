#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define PORT "42069"
#define LEN 88

void *get_addr(struct sockaddr *sa)
{
        if (sa->sa_family == AF_INET) {
                return &(((struct sockaddr_in *)sa)->sin_addr);
        }

        return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(void)
{
        int retval = -1;
        int fd = -1;
        int bufsize = 0;
        char buf[LEN];
        char ip_str[INET6_ADDRSTRLEN];
        socklen_t client_addrlen;
        struct addrinfo hints, *res, *current;
        struct sockaddr_storage client_addr;

        memset(&hints, 0, sizeof(hints));

        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_flags = AI_PASSIVE;

        if ((retval = getaddrinfo(NULL, PORT, &hints, &res)) != 0) {
                fprintf(stderr, "getaddrinfo: %s", gai_strerror(retval));
                return 2;
        }

        for (current = res; current != NULL; res = res->ai_next) {
                if ((fd = socket(current->ai_family, current->ai_socktype, current->ai_protocol)) == -1) {
                        perror("socket");
                        continue;
                }

                if (bind(fd, current->ai_addr, current->ai_addrlen) == -1) {
                        perror("bind");
                        close(fd);
                        continue;
                }

                break;
        }

        freeaddrinfo(res);

        if (current == NULL) {
                fprintf(stderr, "No available socket to bind");
                exit(1);
        }

        printf("Listening port: %s\n", PORT);

        client_addrlen = sizeof client_addr;

        if ((bufsize = recvfrom(fd, buf, LEN-1, 0, (struct sockaddr *)&client_addr, &client_addrlen)) == -1) {
                perror("recvfrom");
                exit(1);
        }

        inet_ntop(client_addr.ss_family, get_addr((struct sockaddr *)&client_addr), ip_str, sizeof(ip_str));
        printf("New connection from: %s\n", ip_str);
        buf[bufsize] = '\0';
        printf("Receiving %d bytes message: %s\n", bufsize, buf);
        printf("Cutting the connection from server...\n");
        close(fd);

        return 0;
}
