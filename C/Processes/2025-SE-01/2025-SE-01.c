#include <err.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

struct processDescription {

    uint16_t ramSize;
    uint16_t registerCount;
    char fileName[8];

};

struct instruction {

    uint8_t opcode;
    uint8_t op1;
    uint8_t op2;
    uint8_t op3;
};

int saveRead(int fd, void* buffer, int size);
int saveRead(int fd, void* buffer, int size) {

    int readSize;
    if ((readSize = read(fd, buffer, size)) < 0) {
        err(1, "system problem with read");
    }

    return readSize;
}

int saveWrite(int fd, void* buffer, int size);
int saveWrite(int fd, void* buffer, int size) {

    int writeSize;
    if ((writeSize = write(fd, buffer, size)) < 0) {
        err(1, "system problem with write");
    }

    return writeSize;
}

void handleOpcode(struct instruction current, uint8_t registers[], uint8_t ramMemory[], int fd);
void handleOpcode(struct instruction current, uint8_t registers[], uint8_t ramMemory[], int fd) {

    switch (current.opcode) {

    case 0:
        registers[current.op1] = registers[current.op2] & registers[current.op3];
        break;
    case 1:
        registers[current.op1] = registers[current.op2] | registers[current.op3];
        break;
    case 2:
        registers[current.op1] = registers[current.op2] + registers[current.op3];
        break;
    case 3:
        registers[current.op1] = registers[current.op2] * registers[current.op3];
        break;
    case 4:
        registers[current.op1] = registers[current.op2] ^ registers[current.op3];
        break;
    case 5:
        saveWrite(1, &registers[current.op1], sizeof(uint8_t));
        break;
    case 6:
        sleep(registers[current.op1]);
        break;
    case 7:
        registers[current.op1] = ramMemory[registers[current.op2]];
        break;
    case 8:
        ramMemory[registers[current.op2]] = registers[current.op1];
        break;
    case 9:
        if (registers[current.op1] != registers[current.op2]) {
            if (lseek(fd, current.op3 * sizeof(struct instruction), SEEK_SET) < 0) {
                err(1, "problem with lseek");
            }
        }
        break;
    case 10:
        registers[current.op1] = current.op2;
        break;
    case 11:
        ramMemory[registers[current.op1]] = current.op2;
        break;
    }
}

void handleFile(struct processDescription current);
void handleFile(struct processDescription current) {

    if (current.ramSize > 512) {
        errx(1, "invalid ram size");
    }

    if (current.registerCount > 32) {
        errx(1, "invalid register count");
    }

    int fileNameSize = 0;
    while (current.fileName[fileNameSize] != '\0') {
        fileNameSize++;
    }

    pid_t pid = fork();
    if (pid < 0) {
        err(1, "problem with fork");
    }

    if (pid == 0) {

        uint8_t registers[32];
        uint8_t ramMemory[512];

        char realFileName[fileNameSize + 1];
        strcpy(realFileName, current.fileName);

        int fileFd = open(realFileName, O_RDWR);
        if (fileFd < 0) {
            err(1, "problem with file fd");
        }

        saveRead(fileFd, registers, current.ramSize);
        saveRead(fileFd, ramMemory, current.registerCount);

        struct instruction currentInstr;

        while (saveRead(fileFd, &currentInstr, sizeof(struct instruction)) > 0) {

            handleOpcode(currentInstr, registers, ramMemory, fileFd);

        }

        if (lseek(fileFd, 0, SEEK_SET) < 0) {
            err(1, "problem with lseek");
        }

        saveWrite(fileFd, registers, current.registerCount);
        saveWrite(fileFd, ramMemory, current.ramSize);

        close(fileFd);
    }
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        errx(1, "there must be 1 arg");
    }

    int inputFd = open(argv[1], O_RDONLY);
    if (inputFd < 0) {
        err(1, "problem with input fd");
    }

    struct stat inputStat;
    if (fstat(inputFd, &inputStat) < 0) {
        err(1, "problem with stat fd");
    }

    int fileSize = inputStat.st_size;

    if ((fileSize % sizeof(struct processDescription)) != 0) {
        errx(1, "input file doesn't have proper length");
    }

    struct processDescription current;

    while (saveRead(inputFd, &current, sizeof(struct processDescription))) {
        handleFile(current);
    }

    int fileNum = fileSize / sizeof(struct processDescription);
    for (int i = 0; i < fileNum; i++) {
        wait(NULL);
    }
}
