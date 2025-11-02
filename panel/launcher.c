#include <fcntl.h>
#include <sched.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

int main(void)
{
    // double fork
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        return 1;
    }
    if (pid > 0)
        return 0;

    setsid();

    pid = fork();
    if (pid < 0) {
        perror("fork failed");
        return 1;
    }
    if (pid > 0)
        return 0;

    // redirect io to /dev/null
    int dev_null = open("/dev/null", O_RDWR);
    dup2(dev_null, STDIN_FILENO);
    dup2(dev_null, STDOUT_FILENO);
    dup2(dev_null, STDERR_FILENO);
    close(dev_null);

    char* const args[] = { "sea", "panel", "create", NULL };
    struct timespec sleep_time = {
        .tv_nsec = 500000000,
        .tv_sec = 0
    };

    while (true) {
        pid_t pid = fork();
        if (pid == 0) {
            execvp(args[0], args);
            perror("execvp failed");
            return 1;
        } else if (pid > 0) {
            int status;
            waitpid(pid, &status, 0);
            nanosleep(&sleep_time, NULL);
        } else {
            perror("fork failed");
            return 1;
        }
    }
    return 0;
}
