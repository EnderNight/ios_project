#include "shell.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

// If you are on the server, remove the argument '-fsanitize=address' inside the
// Makefile file!!

struct termios orig_termios;

void disableRawMode(void) { tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios); }

void enableRawMode(void) {

    struct termios raw = orig_termios;
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag &= (unsigned int)~(ECHO | ICANON);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main(int argc, char **argv) {
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

    if (argc > 0 && argc < 3) {

        Shell *shell;
        int debug = 0;

        tcgetattr(STDIN_FILENO, &orig_termios);

        enableRawMode();
        shell = sh_init();

        if (shell == NULL) {
            print("Error while shell bootstrap.");
        } else {
            if (argc == 2)
                debug = atoi(argv[1]);
            if (debug != 0 && debug != 1) {
                print("Incorrect debug value '%d'. Must be either nothing, 0 "
                      "or 1.\n",
                      debug);
                disableRawMode();
                sh_end(shell);
                return 1;
            } else
                // We put the player inside the right directory
                // TODO : should be something else than /test

                // Put the pointer at the top
                print("\033[H");
            // Clear the shell screen
            print("\033[2J");

            // Going to the right directory, will display the story
            chdir("story");
            change_color("cyan");
            print("When you are ready, go to the start directory!\n");
            change_color("white");

            sh_loop(shell, debug);
        }

        sh_end(shell);
        disableRawMode();
        return 0;
    }

    // disableRawMode();
    print("Incorrect number of arguments: ./main <debug switch (optional)>\n");
    return 1;
}
