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
            print("Error while shell bootstrap.\n");
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
            change_color("magenta");
            print("   ('-.      _ (`-.                           ('-.                             _ (`-.   .-')      ('-.   \n");
            print("  ( OO ).-. ( (OO  )                         ( OO ).-.                        ( (OO  ) ( OO ).  _(  OO)  \n");
            print("  / . --. /_.`     \\ .-'),-----.    .-----.  / . --. / ,--.       ,--.   ,--._.`     \\(_)---\\_)(,------. \n");
            print("  | \\-.  \\(__...--''( OO'  .-.  '  '  .--./  | \\-.  \\  |  |.-')    \\  `.'  /(__...--''/    _ |  |  .---' \n");
            print(".-'-'  |  ||  /  | |/   |  | |  |  |  |('-..-'-'  |  | |  | OO ) .-')     /  |  /  | |\\  :` `.  |  |     \n");
            print(" \\| |_.'  ||  |_.' |\\_) |  |\\|  | /_) |OO  )\\| |_.'  | |  |`-' |(OO  \\   /   |  |_.' | '..`''.)(|  '--.  \n");
            print("  |  .-.  ||  .___.'  \\ |  | |  | ||  |`-'|  |  .-.  |(|  '---.' |   /  /\\_  |  .___.'.-._)   \\ |  .--'  \n");
            print("  |  | |  ||  |        `'  '-'  '(_'  '--'\\  |  | |  | |      |  `-./  /.__) |  |     \\       / |  `---. \n");
            print("  `--' `--'`--'          `-----'    `-----'  `--' `--' `------'    `--'      `--'      `-----'  `------' \n\n");
            change_color("yellow");
            sleep(1);
            change_color("italic");
            print("When you are ready, go to the start directory!\n");
            print("If you are confused, use 'man' to get help!\n");
            change_color("white");

            sh_loop(shell, debug);
            sh_end(shell);
        }

        disableRawMode();
        return 0;
    }

    // disableRawMode();
    print("Incorrect number of arguments: ./main <debug switch (optional)>\n");
    return 1;
}
