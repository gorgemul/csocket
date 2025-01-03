#include "../lib/unity/unity.h"
#include "../include/ps.h"
#include "../include/constants.h"
#include <unistd.h>
#include <stdlib.h>

void setUp(void)
{
}

void tearDown(void)
{
}

void test_init_listener(void)
{
        int fd = -1;
        fd = init_listener(PORT, BACKLOG);
        TEST_ASSERT_TRUE(fd > 2);

        fd > 2 && close(fd);
}

void test_append_fd_same_size_1(void)
{
        int count = 0;
        int size = 5;
        int new_fd = 88;

        struct pollfd *pfds = malloc(sizeof(*pfds) * size);

        append_fd(&pfds, new_fd, &count, &size);

        TEST_ASSERT_EQUAL_INT(pfds[0].fd, new_fd);
        TEST_ASSERT_EQUAL(pfds[0].events, POLLIN);

        TEST_ASSERT_EQUAL_INT(count, 1);
        TEST_ASSERT_EQUAL_INT(size, 5);

        free(pfds);
}

void test_append_fd_same_size_2(void)
{
        int count = 0;
        int size = 5;
        int new_fd = 88;

        struct pollfd *pfds = malloc(sizeof(*pfds) * size);

        pfds[0].fd = 77;
        pfds[0].events = POLLIN;
        count++;

        append_fd(&pfds, new_fd, &count, &size);

        TEST_ASSERT_EQUAL_INT(pfds[1].fd, new_fd);
        TEST_ASSERT_EQUAL(pfds[1].events, POLLIN);

        TEST_ASSERT_EQUAL_INT(count, 2);
        TEST_ASSERT_EQUAL_INT(size, 5);

        free(pfds);
}

void test_append_fd_double_size(void)
{
        int count = 5;
        int size = 5;
        int new_fd = 88;

        struct pollfd *pfds = malloc(sizeof(*pfds) * size);

        append_fd(&pfds, new_fd, &count, &size);

        TEST_ASSERT_EQUAL_INT(pfds[5].fd, new_fd);
        TEST_ASSERT_EQUAL(pfds[5].events, POLLIN);

        TEST_ASSERT_EQUAL_INT(count, 6);
        TEST_ASSERT_EQUAL_INT(size, 10);

        free(pfds);
}

void test_remove_fd_last_element_1(void) {
        int count = 0;
        int size = 5;

        struct pollfd *pfds = malloc(sizeof(*pfds) * size);

        pfds[0].fd = 88;
        pfds[0].events = POLLIN;
        count++;

        remove_fd(&pfds, 0, &count);

        TEST_ASSERT_EQUAL_INT(count, 0);

        free(pfds);
}

void test_remove_fd_last_element_2(void) {
        int count = 0;
        int size = 5;

        struct pollfd *pfds = malloc(sizeof(*pfds) * size);

        pfds[0].fd = 77;
        pfds[0].events = POLLIN;
        count++;

        pfds[1].fd = 88;
        pfds[1].events = POLLIN;
        count++;

        remove_fd(&pfds, 1, &count);

        TEST_ASSERT_EQUAL_INT(count, 1);

        free(pfds);
}

void test_remove_fd_not_last_element_1(void) {
        int count = 0;
        int size = 5;

        struct pollfd *pfds = malloc(sizeof(*pfds) * size);

        pfds[0].fd = 77;
        pfds[0].events = POLLIN;
        count++;

        pfds[1].fd = 88;
        pfds[1].events = POLLIN;
        count++;

        remove_fd(&pfds, 0, &count);

        TEST_ASSERT_EQUAL_INT(pfds[0].fd, 88);
        TEST_ASSERT_EQUAL_INT(count, 1);

        free(pfds);
}

void test_remove_fd_not_last_element_2(void) {
        int count = 0;
        int size = 5;

        struct pollfd *pfds = malloc(sizeof(*pfds) * size);

        pfds[0].fd = 77;
        pfds[0].events = POLLIN;
        count++;

        pfds[1].fd = 88;
        pfds[1].events = POLLIN;
        count++;

        pfds[2].fd = 99;
        pfds[2].events = POLLIN;
        count++;

        pfds[3].fd = 111;
        pfds[3].events = POLLIN;
        count++;

        remove_fd(&pfds, 1, &count);

        TEST_ASSERT_EQUAL_INT(pfds[1].fd, 111);
        TEST_ASSERT_EQUAL_INT(count, 3);

        free(pfds);
}

int main(void)
{
        UNITY_BEGIN();

        RUN_TEST(test_init_listener);
        RUN_TEST(test_append_fd_same_size_1);
        RUN_TEST(test_append_fd_same_size_2);
        RUN_TEST(test_append_fd_double_size);
        RUN_TEST(test_remove_fd_last_element_1);
        RUN_TEST(test_remove_fd_last_element_2);
        RUN_TEST(test_remove_fd_not_last_element_1);
        RUN_TEST(test_remove_fd_not_last_element_2);

        UNITY_END();

        return 0;
}
