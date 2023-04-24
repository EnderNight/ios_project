#include "shell.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// If you are on the server, remove the argument '-fsanitize=address' inside the
// Makefile file!!

int main(void) {
    // Clear the shell screen
    // write(0, "\033[2J", 4);
    // write(0, "\033[H", 3);

    ////Change the shell color
    // change_color("green");
    // write(STDOUT_FILENO, "You woke up at 13 pm and are ready to start your
    // day.\n", 56); sleep(2); write(STDOUT_FILENO, "As you listen to the news
    // you realise that you have 24 hours left before the city gets nuked.\n",
    // 94); sleep(2); write(STDOUT_FILENO, "“Sure whatever” you think.\n", 32);
    // sleep(2);
    // write(STDOUT_FILENO, "You go outside and realize that there is not a
    // single person around.\n", 69); sleep(2); write(STDOUT_FILENO, "By the
    // time you fully wake up, you witness bodies, blood, and fire. Nothing like
    // usual.\n", 88); sleep(2); write(STDOUT_FILENO, "Despite feeling lost and
    // confused, you tell yourself one thing :\n", 66); sleep(2);
    // write(STDOUT_FILENO, "\n“I have to get out of here.”\n\n",36);

    ////Execute the man page, to tell player what he can do
    ////TODO

    ////Change the shell color
    // change_color("white");

    char *Prompt = "Outside> ";
    int eof = 0;
    int argc;
    char *args[MAXARGS];

    while (1) {
        write(0, Prompt, strlen(Prompt));
        if (read_args(&argc, args, MAXARGS, &eof) && argc > 0) {
            sh_execute(argc, args);
        }
        if (eof)
            exit(0);
    }
}
