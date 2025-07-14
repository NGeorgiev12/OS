#include <err.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>

bool isTimeToStop(bool isDone[], int exitCode, int border, int processTime);
bool isTimeToStop(bool isDone[], int exitCode, int border, int processTime) {

    if (exitCode != 0) {

        if (isDone[0] == true) {
            isDone[1] = true;
            return (processTime < border);
        }

        isDone[0] = true;

    }
    else {

        isDone[0] = false;
        isDone[1] = false;
    }

    return false;
}

void writeToFile(int fileFd, long procStart, long procEnd, int exitCode);
void writeToFile(int fileFd, long procStart, long procEnd, int exitCode) {

    char buf[128];
    int readSize;
    if ((readSize = snprintf(buf, sizeof(buf), "%ld %ld %d\n", procStart, procEnd, exitCode)) < 0) {
        err(1, "problem with snprintf");
    }


    if (write(fileFd, buf, readSize) != readSize) {
        err(1, "cannot write");
    }

}

int main(int argc, char* argv[]) {

    if (argc < 3) {
        errx(1, "args must be at least 3");
    }

    char* endptr;
    int border = strtol(argv[1], &endptr, 10);

    char command[128];
    strcpy(command, argv[2]);

    if (border < 1 || border > 9) {
        errx(1, "border must be in the range 1-9");
    }

    int runFd = open("run.log", O_WRONLY | O_CREAT | O_APPEND, 0644);

    if (runFd < 0) {
        err(1, "cannot open run.log");
    }

    bool isDone[] = { 0, 0 };

    while (true) {

        time_t start = time(NULL);
        pid_t pid = fork();

        if (pid < 0) {
            err(1, "cannot fork");
        }

        if (pid == 0) {

            execvp(command, argv + 2);
            err(1, "cannot execute command");

        }
        else {

            int status;
            int chPid = wait(&status);

            if (chPid < 0) {
                err(1, "cannot wait");
            }

            time_t end = time(NULL);

            int processTime = end - start;

            int exitCode = 0;
            if (WIFEXITED(status)) {
                exitCode = WEXITSTATUS(status);
            }
            else if (WIFSIGNALED(status)) {
                exitCode = 129;
            }

            writeToFile(runFd, start, end, exitCode);

            if (isTimeToStop(isDone, exitCode, border, processTime)) break;
        }
    }

    close(runFd);

}
