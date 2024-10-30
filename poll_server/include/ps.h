#include <poll.h>

#ifndef PS_H
#define PS_H

int init_listener(const char *port, const int backlog);
void append_fd(struct pollfd **pfds, int fd, int *count, int *size);
void remove_fd(struct pollfd **pfds, int fd, int *count, int *size);

#endif
