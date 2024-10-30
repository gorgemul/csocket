#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/wait.h>

#define PORT "42069"
#define QUEUELEN 15

void reap_zombie_child_process(int sig)
{
        (void)sig;
        int saved_errno = errno;
        while (waitpid(-1, NULL, WNOHANG) > 0);
        errno = saved_errno;
}

void *get_addr(struct sockaddr *sa)
{
        if (sa->sa_family == AF_INET) {
                return &(((struct sockaddr_in *)sa)->sin_addr);
        }

        return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(void)
{
        int retval = 0;
        int yes = 1;
        int fd = -1;
        int new_fd = -1;
        socklen_t client_addr_length;
        char client_ip_str[INET6_ADDRSTRLEN];
        struct addrinfo hints, *res, *current;
        struct sockaddr_storage client_addr;
        struct sigaction sa;

        printf("Waiting for connections...\n");

        memset(&hints, 0, sizeof(hints));

        hints.ai_flags = AI_PASSIVE;
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        if ((retval = getaddrinfo(NULL, PORT, &hints, &res)) != 0) {
                fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(retval));
                return 1;
        }

        for (current = res; current != NULL; res = res->ai_next) {
                if ((fd = socket(current->ai_family, current->ai_socktype, current->ai_protocol)) == -1) {
                        perror("socket");
                        continue;
                }

                if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
                        perror("setsockopt");
                        close(fd);
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
                fprintf(stderr, "No available socket to bind\n");
                exit(1);
        }

        if (listen(fd, QUEUELEN) == -1) {
                perror("listen");
                close(fd);
                exit(1);
        }

        // Clear zombie child process
        sa.sa_handler = reap_zombie_child_process;
        sa.sa_flags = SA_RESTART;
        sigemptyset(&(sa.sa_mask));

        if (sigaction(SIGCHLD, &sa, NULL) == -1) {
                perror("sigaction");
                close(fd);
                exit(1);
        }

        printf("Listening port: %s\n", PORT);

        while (1) {
                client_addr_length = sizeof(client_addr);
                if ((new_fd = accept(fd, (struct sockaddr *)&client_addr, &client_addr_length)) == -1) {
                        perror("send");
                        continue;
                }

                inet_ntop(client_addr.ss_family, get_addr((struct sockaddr *)&client_addr), client_ip_str, sizeof(client_ip_str));

                printf("New connection from: %s\n", client_ip_str);

                // Child context
                if (!fork()) {
                        char *msg = "Hello, client!";
                        close(fd);
                        if (send(new_fd, msg, strlen(msg), 0) == -1) {
                                perror("send");
                        }

                        close(new_fd);
                        exit(0);
                }

                close(new_fd);
        }


        return 0;
}
