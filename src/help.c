#include "help.h"
#include <fcntl.h> //for using open and read system calls for files
#include <stdio.h>
#include <stdlib.h>
void help() {

    // fdold will save the indicator that the file has been opened
    FILE *fp;
    char line[MAX_LINE_LENGTH];
    // we open the file
    fp = fopen("instructions.txt", "r");
    // check
    if (fp == NULL) {
        printf("cannot open file");
        exit(1);
    }
    // if file is opened then write on the  buffer the content of the file
    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
        printf("%s", line);
    }
}
int main(void) {
    help();
    return 0;
}
