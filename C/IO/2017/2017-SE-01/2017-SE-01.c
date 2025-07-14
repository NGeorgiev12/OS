#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <err.h>
#include <sys/stat.h>

struct ptriple {

    uint16_t offset;
    uint8_t origByte;
    uint8_t newByte;
};

int main(int argc, char* argv[]) {

    if (argc != 4) {
        err(1, "args must be 3");
    }

    int f1Fd = open(argv[1], O_RDONLY);

    if (f1Fd < 0) {
        err(1, "cannot read from f1 file");
    }

    int f2Fd = open(argv[2], O_RDONLY);

    if (f2Fd < 0) {
        err(1, "cannot read from f2 file");
    }

    int patchFd = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (patchFd < 0) {
        err(1, "cannot write in patch file");
    }

    struct stat f1Info;
    int f1InfoFd = fstat(f1Fd, &f1Info);

    if (f1InfoFd < 0) {
        err(1, "cannot open f1 stat");
    }

    struct stat f2Info;
    int f2InfoFd = fstat(f2Fd, &f2Info);

    if (f2InfoFd < 0) {
        err(1, "cannot open f2 stat");
    }

    if (f1Info.st_size != f2Info.st_size) {
        errx(1, "the f1 and f2 files must have equal length");
    }

    int readSize1;
    int readSize2;
    uint8_t currentF1Char;
    uint8_t currentF2Char;

    while (((readSize1 = read(f1Fd, &currentF1Char, 1)) > 0) && (readSize2 = read(f2Fd, &currentF2Char, 1)) > 0) {

        if (currentF1Char != currentF2Char) {
            struct ptriple patchTriple;
            patchTriple.offset = (lseek(f1Fd, 0, SEEK_CUR) - 1);
            patchTriple.origByte = currentF1Char;
            patchTriple.newByte = currentF2Char;

            if (write(patchFd, &patchTriple, sizeof(struct ptriple)) < 0) {
                err(1, "cannot write in patch file");
            }
        }
    }

    if (readSize1 < 0) {
        err(1, "cannot read from f1");
    }

    if (readSize2 < 0) {
        err(1, "cannot read from f2");
    }

    close(f1Fd);
    close(f2Fd);
    close(patchFd);

}
