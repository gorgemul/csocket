#include "../include/ps.h"
#include "../include/constants.h"
#include <poll.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int main(void)
{
        int fd = -1;
        int size = 5;
        int count = 0;

        struct pollfd *pfds = malloc(sizeof(*pfds) * size);

        fd = init_listener(PORT, BACKLOG);
        printf("Listening port: %s\n", PORT);

        append_fd(&pfds, fd, &count, &size);

        while (1) {
        }

        close(fd);
        free(pfds);

        return 0;
}
