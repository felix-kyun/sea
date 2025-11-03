#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

static int log_fd = -1;

void open_log(void)
{
    if (log_fd != -1)
        return;

    log_fd = open("/tmp/launcher.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (log_fd == -1) {
        perror("Failed to open log file");
        exit(1);
    }

    dup2(log_fd, STDOUT_FILENO);
    dup2(log_fd, STDERR_FILENO);
}

int main(void)
{
    open_log();

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

    int dev_null = open("/dev/null", O_RDWR);
    dup2(dev_null, STDIN_FILENO);
    close(dev_null);

    char* const args[] = { "sea", "panel", "create", NULL };

    while (true) {
        pid_t pid = fork();
        if (pid == 0) {
            execvp(args[0], args);
            perror("execvp failed");
            return 1;
        } else if (pid > 0) {
            int status;
            waitpid(pid, &status, 0);
            sleep(1);
        } else {
            perror("fork failed");
            return 1;
        }
    }

    close(log_fd);
    return 0;
}
