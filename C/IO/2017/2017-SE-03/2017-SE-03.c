#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <err.h>
#include <fcntl.h>
#include <sys/stat.h>

struct ptriple {

    uint16_t offset;
    uint8_t oldByte;
    uint8_t newByte;

};

int main(int argc, char* argv[]) {

    if (argc != 4) {
        err(1, "args must be 3");
    }

    int patchFd = open(argv[1], O_RDONLY);
    if (patchFd < 0) {
        err(1, "could not open patch file");
    }

    int f1Fd = open(argv[2], O_RDONLY);

    if (f1Fd < 0) {
        err(1, "could not open f1 file");
    }

    int f2Fd = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (f2Fd < 0) {
        err(1, "could not open f2 file");
    }

    struct stat infoPatch;
    int infoPatchFd = fstat(patchFd, &infoPatch);

    if (infoPatchFd < 0) {
        err(1, "could not open patch stat file");
    }

    int patchSize = infoPatch.st_size;

    if ((patchSize % 4) != 0) {
        err(1, "patch file is invalid");
    }

    struct stat infoF1;
    int infoF1Fd = fstat(f1Fd, &infoF1);

    if (infoF1Fd < 0) {
        err(1, "could not open f1 stat file");
    }

    int f1Size = infoF1.st_size;

    if ((f1Size % 2) != 0) {
        err(1, "f1 file has invalid length");
    }

    char f1Buf[2];
    int numBytes1;
    while ((numBytes1 = read(f1Fd, f1Buf, 2)) > 0) {
        write(f2Fd, f1Buf, strlen(f1Buf));
    }

    int numBytes2;
    struct ptriple patchTriple;
    while ((numBytes2 = read(patchFd, &patchTriple, 4)) > 0) {

        if (patchTriple.offset > f1Size) {
            errx(1, "invalid patch offset");
        }

        if (lseek(f1Fd, patchTriple.offset, SEEK_SET) < 0) {
            err(1, "cannot move file pointer");
        }

        int f1Byte;
        if ((read(f1Fd, &f1Byte, 1)) < 0) {
            err(1, "cannot read char from f1");
        }

        if (f1Byte != patchTriple.oldByte) {
            errx(1, "different old bytes in f1 and patch");
        }

        if (lseek(f2Fd, patchTriple.offset, SEEK_SET) < 0) {
            err(1, "could not move file pointer in fd");
        }

        if (write(f2Fd, &patchTriple.newByte, 1) < 0) {
            err(1, "could not write in f2 file");
        }
    }
    close(patchFd);
    close(f1Fd);
    close(f2Fd);
}