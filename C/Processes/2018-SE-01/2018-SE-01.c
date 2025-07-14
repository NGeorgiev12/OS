#include <err.h>
#include <unistd.h>
#include <sys/wait.h>

void doChild(int index, char dir[]);
void doChild(int index, char dir[]) {

    if (index == 0) {

        execlp("find", "find", dir, "-type", "f", "-printf", "%p %T@\n", (char*)NULL);
        err(1, "cannot execute find");

    }
    else if (index == 1) {

        execlp("sort", "sort", "-n", "-k", "2", (char*)NULL);
        err(1, "cannot execute sort");

    }
    else if (index == 2) {

        execlp("head", "head", "-n", "1", (char*)NULL);
        err(1, "cannot execute head");

    }
    else {

        execlp("cut", "cut", "-d", " ", "-f", "1", (char*)NULL);
        err(1, "cannot execute cut");

    }
}

void closePipes(int pipes[][2]);
void closePipes(int pipes[][2]) {

    for (int i = 0; i < 3; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        errx(1, "there must be 1 arg");
    }

    int pipes[3][2];

    for (int i = 0; i < 3; i++) {
        if (pipe(pipes[i]) < 0) {
            err(1, "cannot pipe");
        }
    }

    for (int i = 0; i < 4; i++) {

        pid_t pid = fork();

        if (pid < 0) {
            err(1, "cannot fork");
        }

        if (pid == 0) {

            if (i > 0) {

                if (dup2(pipes[i - 1][0], 0) < 0) {
                    err(1, "cannot dup2");
                }
            }

            if (i < 3) {

                if (dup2(pipes[i][1], 1) < 0) {
                    err(1, "cannot dup2");
                }
            }

            closePipes(pipes);
            doChild(i, argv[1]);
        }

    }

    closePipes(pipes);

    for (int i = 0; i < 4; i++) {
        wait(NULL);
    }
}
