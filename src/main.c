#include "shell.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// If you are on the server, remove the argument '-fsanitize=address' inside the
// Makefile file!!

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
            change_color("cyan");
            sleep(1);
            print("When you are ready, go to the start directory!\n");
            change_color("white");
            sh_loop(shell, debug);
        }

        sh_end(shell);
        return 0;
    }

    print("Incorrect number of arguments: ./main <debug switch (optional)>\n");
    return 1;
}
