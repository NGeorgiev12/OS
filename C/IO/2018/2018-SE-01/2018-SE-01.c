#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <err.h>

bool isCharInArray(char ch, char arr[], size_t size) {

    for (size_t i = 0; i < size; i++) {

        if (arr[i] == ch) {
            return true;
        }

    }

    return false;
}

int main(int argc, char* argv[]) {

    if (argc != 3) {
        err(1, "args must be 2");
    }

    char option[3];
    char set1[256];
    char set2[256];

    if (strcmp(argv[1], "-d") == 0 || strcmp(argv[1], "-s") == 0) {

        strcpy(option, argv[1]);
        strcpy(set1, argv[2]);

    }
    else {

        strcpy(set1, argv[1]);
        strcpy(set2, argv[2]);

        if (strlen(set1) != strlen(set2)) {
            errx(1, "two sets must have equal size");
        }

    }

    size_t setSize = strlen(set1);
    int readSize;
    char current;
    char previousChar = '\0';

    bool isDelete = (strcmp(option, "-d") == 0);
    bool isSqueeze = (strcmp(option, "-s") == 0);
    bool isTranslate = !isDelete && !isSqueeze;

    while ((readSize = read(0, &current, 1)) > 0) {

        if (isTranslate) {

            for (size_t i = 0; i < setSize; i++) {
                if (current == set1[i]) {
                    current = set2[i];
                    break;
                }
            }

        }
        else if (isDelete) {

            if (isCharInArray(current, set1, setSize)) {
                continue;
            }

        }
        else if (isSqueeze) {

            char squeezeSymbol = set1[0];

            if (previousChar == current && current == squeezeSymbol) {
                previousChar = current;
                continue;
            }

            previousChar = current;
        }

        if (write(1, &current, readSize) < 0) {
            err(1, "cannot write in stdout");
        }

    }

    if (readSize < 0) {
        err(1, "cannot read from stdin");
    }
}
