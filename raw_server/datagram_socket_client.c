#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define PORT "42069"

int main(int argc, char **argv)
{
        int retval = -1;
        int fd = -1;
        int bufsize = -1;
        char *msg = "Hello from client";

        struct addrinfo hints, *res, *current;

        memset(&hints, 0, sizeof(hints));
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_family = AF_INET;

        if (argc != 2) {
                fprintf(stderr, "Expect an ip address as argument!\n");
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

                break;
        }

        if (current == NULL) {
                fprintf(stderr, "No available socket to bind\n");
                exit(1);
        }

        if ((bufsize = sendto(fd, msg, strlen(msg), 0, current->ai_addr, current->ai_addrlen)) == -1) {
                perror("sendto");
                freeaddrinfo(res);
                close(fd);
                exit(1);
        }

        freeaddrinfo(res);
        printf("Sending %d bytes message(%s) to %s:%s\n", bufsize, msg, argv[1], PORT);
        close(fd);

        return 0;
}
