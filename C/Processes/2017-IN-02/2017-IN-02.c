#include <err.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/wait.h>

bool isDelim(char ch);
bool isDelim(char ch) {
    return (ch == '\n' || ch == ' ');
}

bool readBuffer(char arr[], int arrSize);
bool readBuffer(char arr[], int arrSize) {

    int readSize;
    int arrIndex = 0;
    char currentChar;

    while (((readSize = read(0, &currentChar, 1)) > 0) && (!isDelim(currentChar))) {

        if (arrIndex >= arrSize) {
            errx(1, "the input string should be max 4 bytes");
        }

        arr[arrIndex++] = currentChar;
    }

    if (readSize < 0) {
        err(1, "cannot read from stdin");
    }

    arr[arrIndex] = '\0';

    if (readSize == 0) {
        return false;
    }

    return true;
}

int main(int argc, char* argv[]) {

    char command[4 + 1];

    if (argc > 2) {
        errx(1, "args must be 0 or 1");
    }

    if (argc == 2) {

        if (strlen(argv[1]) > 4) {
            errx(1, "command should have max size 4 bytes");
        }

        strcpy(command, argv[1]);


    }
    else {

        strcpy(command, "echo");
    }

    const int bufSize = 4 + 1;
    char buf1[bufSize];
    char buf2[bufSize];


    while (true) {

        if (!readBuffer(buf1, bufSize)) {
            break;
        }

        bool secondRead = readBuffer(buf2, bufSize);

        pid_t pid = fork();

        if (pid < 0) {
            err(1, "could not fork");
        }

        if (pid == 0) {

            if (!secondRead || strlen(buf2) == 0) {
                if (execlp(command, command, buf1, (char*)NULL) < 0) {
                    err(1, "cannot execute the command");
                }
            }
            else {
                if (execlp(command, command, buf1, buf2, (char*)NULL) < 0) {
                    err(1, "cannot execute the command");
                }
            }

            exit(0);

        }
        else {

            int status;
            int childPid = wait(&status);
            if (childPid < 0) {
                err(1, "problem with wait");
            }
        }

    }
}
