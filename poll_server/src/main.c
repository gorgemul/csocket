#include "../include/ps.h"
#include "../include/constants.h"
#include <poll.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(void)
{
        int listener = -1;
        int new_fd = -1;
        int size = 5;
        int count = 0;
        int ready = -1;
        int recv_bytes = -1;

        char recv_msg[MSGLENGTH];

        struct sockaddr_storage remote_addr;
        socklen_t remote_addrlen;
        char remote_ip[INET6_ADDRSTRLEN];

        struct pollfd *pfds = malloc(sizeof(*pfds) * size);

        listener = init_listener(PORT, BACKLOG);
        printf("Listening port: %s\n", PORT);

        append_fd(&pfds, listener, &count, &size);

        while (1) {
                if ((ready = poll(pfds, count, -1)) == -1) {
                        perror("poll");
                        free(pfds);
                        exit(1);
                }

                if (ready == 0) {
                        printf("timeout\n");
                        free(pfds);
                        return 0;
                }

                for (int i = 0; i < count; i++) {
                        int is_listener = (pfds[i].fd == listener);
                        int readable = (pfds[i].revents & POLLIN);

                        if (!readable)
                                continue;

                        if (is_listener) {
                                remote_addrlen = sizeof(remote_addr);
                                if ((new_fd = accept(listener, (struct sockaddr *)&remote_addr, &remote_addrlen)) == -1) {
                                        perror("accept");
                                        continue;
                                }

                                printf("New connection from: %s, assigning new fd: %d\n", inet_ntop(remote_addr.ss_family ,get_addr((struct sockaddr *)&remote_addr), remote_ip, INET6_ADDRSTRLEN), new_fd);

                                append_fd(&pfds, new_fd, &count, &size);
                        }

                        if (!is_listener) {
                                recv_bytes = recv(pfds[i].fd, recv_msg, MSGLENGTH-1, 0);

                                switch (recv_bytes) {
                                case -1:
                                        perror("recv");
                                        continue;
                                case 0:
                                        remove_fd(&pfds, i, &count);
                                        printf("Remote client(socket: %d) disconnect from poll server\n", pfds[i].fd);
                                        printf("Close socket(%d) from server...\n", pfds[i].fd);
                                        printf("Remaining connection: %d\n", count);
                                        break;
                                default:
                                        recv_msg[recv_bytes] = '\0';
                                        printf("Receiving message from client(socket %d): %s", pfds[i].fd, recv_msg);
                                        printf("Broadcasting the message to clients ========>\n");
                                        broadcast_message(pfds[i].fd, listener, recv_msg, &pfds, count);
                                        break;
                                }
                        }
                }
        }

        free(pfds);

        return 0;
}
