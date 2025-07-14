#include <err.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdint.h>

void swap(uint32_t* num1, uint32_t* num2);
void swap(uint32_t* num1, uint32_t* num2) {
    uint32_t num3 = *num1;
    *num1 = *num2;
    *num2 = num3;
}

void selectionSort(uint32_t arr[], size_t size);
void selectionSort(uint32_t arr[], size_t size) {

    for (size_t i = 0; i < size; i++) {

        size_t minIndex = i;

        for (size_t j = i + 1; j < size; j++) {
            if (arr[j] < arr[minIndex]) {
                minIndex = j;
            }
        }

        if (minIndex != i) {
            swap(&arr[i], &arr[minIndex]);
        }

    }
}

int main(int argc, char* argv[]) {

    if (argc != 3) {
        err(1, "args must be 3");
    }

    int f1Fd = open(argv[1], O_RDONLY);

    if (f1Fd < 0) {
        err(1, "cannot read from f1");
    }

    int f2Fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (f2Fd < 0) {
        err(1, "cannot write in f2");
    }

    struct stat f1Info;
    int f1InfoFd = fstat(f1Fd, &f1Info);

    if (f1InfoFd < 0) {
        err(1, "cannot access f1 stat file");
    }

    size_t size = f1Info.st_size;

    if ((size % 4) != 0) {
        errx(1, "invalid f1 file");
    }

    size_t elCount = size / 4;

    if (elCount > 4194304) {
        errx(1, "too much elements");
    }

    uint32_t array[4194304];

    ssize_t bytesRead = read(f1Fd, array, elCount * sizeof(uint32_t));
    if (bytesRead > 4194304) {
        errx(1, "unexpected EOF or file corruption");
    }


    selectionSort(array, elCount);

    for (size_t i = 0; i < elCount; i++) {

        if (write(f2Fd, &array[i], sizeof(uint32_t)) < 0) {
            err(1, "cannot write to f2");
        }
    }

}
