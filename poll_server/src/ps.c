#include "../include/ps.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int init_listener(const char *port, const int backlog)
{
        int retval = -1;
        int fd = -1;
        struct addrinfo hints, *res, *current;

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;

        if ((retval = getaddrinfo(NULL, port, &hints, &res)) != 0) {
                fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(retval));
                exit(1);
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
                fprintf(stderr, "No available socket\n");
                exit(1);
        }

        if (listen(fd, backlog) == -1) {
                perror("listen");
                close(fd);
                exit(1);
        }

        return fd;
}

void append_fd(struct pollfd **pfds, int fd, int *count, int *size)
{
        if (*count == *size) {
                (*size) *= 2;
                struct pollfd *new_pfds = realloc(*pfds, sizeof(**pfds) * (*size));
                if (new_pfds == NULL) {
                        (*size) /= 2;
                        fprintf(stderr, "Realloc having trouble!!!\n");
                        return;
                }

                *pfds = new_pfds;
        }

        (*pfds)[*count].fd = fd;
        (*pfds)[*count].events = POLLIN;

        (*count)++;
}
