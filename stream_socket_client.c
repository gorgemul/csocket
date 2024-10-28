#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define PORT "42069"
#define LEN 88

int main(int argc, char **argv)
{
        int retval = -1;
        int fd = -2;
        int bufsize;
        char buf[LEN];
        struct addrinfo hints, *res, *current;

        printf("Initializing connection\n");

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        if (argc != 2) {
                fprintf(stderr, "Enter an ip as argument!");
                return 1;
        }

        if ((retval = getaddrinfo(argv[1], PORT, &hints, &res)) != 0) {
                fprintf(stderr, "getaddrinfo: %s", gai_strerror(retval));
                return 2;
        }

        for (current = res; current != NULL; res = res->ai_next) {
                if ((fd = socket(current->ai_family, current->ai_socktype, current->ai_protocol)) == -1) {
                        perror("socket");
                        continue;
                }

                if (connect(fd, current->ai_addr, current->ai_addrlen) == -1) {
                        perror("connect");
                        close(fd);
                        continue;
                }

                break;
        }

        freeaddrinfo(res);

        if (current == NULL) {
                fprintf(stderr, "No available connection!\n");
                close(fd);
                return 1;
        }

        printf("Connection established on: %s:%s\n", argv[1], PORT);

        if ((bufsize = recv(fd, buf, LEN, 0))) {
                buf[bufsize] = '\n';
                printf("Message received from server: %s", buf);
        }

        close(fd);

        printf("Cutting the connection.....\n");

        return 0;
}
