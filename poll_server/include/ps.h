#include <poll.h>
#include <sys/socket.h>

#ifndef PS_H
#define PS_H

int init_listener(const char *port, const int backlog);
void append_fd(struct pollfd **pfds, int fd, int *count, int *size);
void remove_fd(struct pollfd **pfds, int pos, int *count);

void *get_addr(struct sockaddr *sa);
void broadcast_message(int sender, int listener, char *msg, struct pollfd **pfds, int count);

#endif
