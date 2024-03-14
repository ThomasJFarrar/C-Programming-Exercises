#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "msString.h"

/* Task 1 */
void printBytes(void *ptr, int numBytes) {
    unsigned char *p = ptr;
    printf("Starting at memory address %p:\n", ptr);
    int i = 0;
    for (i; i < numBytes; i++) {
        printf("%03d: %d\n", i + 1, p[i]);
    }
}

/* Task 2 */
void reverseFile(const char *fileIn, const char *fileOut) {
    FILE *fin = fopen(fileIn, "r");
    FILE *fout = fopen(fileOut, "w");

    if (fin == NULL || fout == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    fseek(fin, 0, SEEK_END);
    long size = ftell(fin);
    fseek(fin, 0, SEEK_SET);

    char *buffer = (char *)malloc(size + 1);
    if (buffer == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    fread(buffer, 1, size, fin);
    buffer[size] = '\0';
    
    long i = size - 1;
    for (i; i >= 0; i--) {
        fputc(buffer[i], fout);
    }

    free(buffer);
    fclose(fin);
    fclose(fout);
}

/* Task 3 */
typedef void * msString;

msString msSetString(char *str) {
    size_t len = strlen(str);
    msString ms = malloc(sizeof(size_t) + len + 1);
    if (ms == NULL) {
        msError("Memory allocation failed");
    }
    *(size_t *)ms = len;
    memcpy((char *)ms + sizeof(size_t), str, len);
    ((char *)ms)[sizeof(size_t) + len] = '\0';
    return ms;
}

char *msGetString(msString ms) {
    return (char *)ms + sizeof(size_t);
}

void msCopy(msString *dest, msString src) {
    size_t len = *(size_t *)src;
    *dest = malloc(sizeof(size_t) + len + 1);
    if (*dest == NULL) {
        msError("Memory allocation failed");
    }
    memcpy(*dest, src, sizeof(size_t) + len);
    *((char *)*dest + sizeof(size_t) + len) = '\0';
}

void msConcatenate(msString *dest, msString src) {
    size_t len_dest = *(size_t *)*dest;
    size_t len_src = *(size_t *)src;
    *dest = realloc(*dest, sizeof(size_t) + len_dest + len_src + 1);
    if (*dest == NULL) {
        msError("Memory allocation failed");
    }
    memcpy((char *)*dest + sizeof(size_t) + len_dest, src + sizeof(size_t), len_src);
    *(size_t *)*dest += len_src;
    ((char *)*dest)[sizeof(size_t) + len_dest + len_src] = '\0';
}

long int msLength(msString ms) {
    return *(size_t *)ms;
}

int msCompare(msString ms1, msString ms2) {
    size_t len1 = *(size_t *)ms1;
    size_t len2 = *(size_t *)ms2;
    if (len1 != len2) {
        return 1;
    }
    return memcmp((char *)ms1 + sizeof(size_t), (char *)ms2 + sizeof(size_t), len1) != 0;
}

int msCompareString(msString ms, char *str) {
    size_t len = strlen(str);
    size_t ms_len = *(size_t *)ms;
    if (len != ms_len) {
        return 1;
    }
    return memcmp((char *)ms + sizeof(size_t), str, len) != 0;
}

static void msError(char *message) {
    fprintf(stderr, "Error: %s\n", message);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s fileIn fileOut\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Task 1 test usage */
    int num = 123456;
    float fnum = 123.456f;
    char str[] = "Hello, World!";
    printBytes(&num, sizeof(num));
    printBytes(&fnum, sizeof(fnum));
    printBytes(str, strlen(str));

    /* Task 2 reverse file content */
    reverseFile(argv[1], argv[2]);

    /* Task 3 test usage */
    msString ms = msSetString("Hello");
    msString ms2 = msSetString(" World!");
    msString mscopy = NULL;

    printf("String |%s| is %ld characters long (%p).\n", msGetString(ms), msLength(ms), ms);
    msCopy(&mscopy, ms);
    printf("Copied string |%s| is %ld characters long (%p).\n", msGetString(mscopy), msLength(mscopy), mscopy);

    printf("Compare ms with mscopy: %d\n", msCompare(ms, mscopy));
    printf("Compare ms with ms2: %d\n", msCompare(ms, ms2));
    printf("Compare ms with Hello: %d\n", msCompareString(ms, "Hello"));
    printf("Compare ms with HelloX: %d\n", msCompareString(ms, "HelloX"));
    printf("Compare ms with Hella: %d\n", msCompareString(ms, "Hella"));

    msConcatenate(&mscopy, ms2);
    printf("Concatenated string |%s| is %ld characters long (%p).\n", msGetString(mscopy), msLength(mscopy), mscopy);

    /* Free dynamically allocated memory to prevent memory leak*/
    free(ms);
    free(mscopy);
    free(ms2);

    return 0;
}

