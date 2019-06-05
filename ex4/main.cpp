#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main() {

    int inputFile = open("input", O_RDONLY);

    if (inputFile < 0) {
        printf("Can't open source file");
        return 1;
    }

    unlink("output.bmp");
    int outputFile = open("output.bmp", O_RDWR | O_CREAT | O_TRUNC, 0666);

    if (outputFile < 0) {
        printf("Can't open destination file");
        return 1;
    }

    off_t fileSize = lseek(inputFile, 0, SEEK_END);
    ftruncate(outputFile, fileSize);

    printf("File size: %lld bytes\n", fileSize);

    long maxChunkSize = sysconf(_SC_PAGE_SIZE) * 256; // 1 MB on macOS

    for (int i = 0; i < fileSize / maxChunkSize + 1; ++i) {
        long offset = i * maxChunkSize;

        long chunkSize = maxChunkSize;

        if (offset + chunkSize > fileSize) {
            chunkSize = fileSize - offset;
        }

        void *srcMapping = mmap(nullptr, (size_t) chunkSize, PROT_READ, MAP_SHARED, inputFile, offset);
        if (srcMapping == MAP_FAILED) {
            printf("Input file can't be mapped on iteration %d (%s)\n", i, strerror(errno));
            return 1;
        }

        void *dstMapping = mmap(nullptr, (size_t) chunkSize, PROT_READ | PROT_WRITE, MAP_SHARED, outputFile, offset);
        if (dstMapping == MAP_FAILED) {
            printf("Output file can't be mapped on iteration %d", i);
            return 1;
        }


        memcpy(dstMapping, srcMapping, (size_t) chunkSize);

        printf("Iteration: %d\n", i);
        printf("From: %ld, bytes: %ld\n", offset, chunkSize);
        printf("\n");

        munmap(srcMapping, (size_t) chunkSize);
        munmap(dstMapping, (size_t) chunkSize);
    }

    close(inputFile);
    close(outputFile);

    return 0;
}