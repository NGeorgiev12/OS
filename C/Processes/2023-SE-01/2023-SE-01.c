#include <err.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>

const int PIPES_COUNT = 2;
const char ESCAPE_CHAR = '\n';
const char EXTENSION[] = ".bash";

void doChild(int index, char command[]);
void doChild(int index, char command[]) {

    if (index == 0) {

        execlp("find", "find", command, "-type", "f", "!", "-name", "*.bash", (char*)NULL);
        err(1, "cannot execute find");

    }
    else if (index == 1) {

        int readSize;
        char current;
        int counter = 0;
        char buffer[64 + 1];

        while ((readSize = read(0, &current, sizeof(char))) > 0) {

            if (current == ESCAPE_CHAR) {
                break;
            }

            buffer[counter++] = current;
        }

        if (readSize < 1) {
            err(1, "problem with read function");
        }

        buffer[counter] = '\0';

        execlp("md5sum", "md5sum", buffer, (char*)NULL);
        err(1, "problem with executing md5sum");

    }
    else if (index == 2) {

        char hash[64 + 1];
        char fileName[64 + 1];

        int hashCount = 0;
        int fileNameCount = 0;
        int readSize;
        char current;

        while ((readSize = read(0, &current, sizeof(char))) > 0) {

            if (current == ' ') {
                break;
            }

            hash[hashCount++] = current;
        }

        if (readSize < 0) {
            err(1, "problem while reading");
        }

        hash[hashCount] = '\0';

        while ((readSize = read(0, &current, sizeof(char))) > 0) {

            if (current == '\n') {
                break;
            }

            fileName[fileNameCount++] = current;
        }

        if (readSize < 0) {
            err(1, "problem while reading");
        }

        strcat(fileName, EXTENSION);

        int outFd = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, 0644);

        if (outFd < 0) {
            err(1, "problem with fd");
        }

        if (write(outFd, hash, hashCount) < 0) {
            err(1, "problem with writing");
        }
    }
}

void closePipes(int pipes[][2], int pipesCount);
void closePipes(int pipes[][2], int pipesCount) {

    for (int i = 0; i < pipesCount; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        errx(1, "there must be 1 arg");
    }

    int pipes[PIPES_COUNT][2];

    for (int i = 0; i < PIPES_COUNT; i++) {

        if (pipe(pipes[i]) < 0) {
            err(1, "cannot create pipe");
        }
    }

    for (int i = 0; i < PIPES_COUNT + 1; i++) {

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

            if (i < PIPES_COUNT) {
                if (dup2(pipes[i][1], 1) < 0) {
                    err(1, "cannot dup2");
                }
            }

            closePipes(pipes, PIPES_COUNT);
            doChild(i, argv[1]);
        }
    }

    closePipes(pipes, PIPES_COUNT);

    for (int i = 0; i < PIPES_COUNT + 1; i++) {
        wait(NULL);
    }

}
