/** @file       main.cpp
    @brief      Director for DEFCon301
    @author     Jonathan Wooding, Lamar University, EE
    @version    1.02.14
*/

/** \todo       SETTING FOR NUM/KEYS
                GEN SETTINGS FILE
                -GAIN
                -SLEEP
                BETTER PRESET HANDLING
                convert to mono <><>
                ^^ done but must be tested with UCA to see if it supports mono stream
                ^^ on my hardware it just records 0 into R channel
                ^^ alternate - record stereo but process mono (after analog summing)
                add interrupt, run forever  <><>
                intensity channels  <><>
                no input cutoff <><>
                ^^ FIXME ^^ in_max <><>
                convert "stereo" to "mono"  ( TEST ON BBB )
                flashing
                ui backspace        <><>
                display range of possible gains
                better descriptions (UI)

*/

#include "stream.h"

void splash(void);

int main(void) {
    bandData        band;       // structure for user's data
    string          fileName;
    int             row, col;   // screen location

    initscr();              // setup terminal screen
    cbreak();               // no need to press enter
    noecho();               // shhhh
    curs_set(0);            // invisible cursor for now
    keypad(stdscr, TRUE);    // arrow keys
    scrollok(stdscr, TRUE); // autoscroll

    splash();       // launch splash screen
    getch();        // pause
    erase();
    curs_set(2);

    /* Main Menu for DEFCon301 */
    /* This will only complete after a user loads a preset */
    /* Creating a new preset internally re-calls Main Menu */
MAIN_MENU:
    fileName = mainMenu();
    if (fileName == "0") {
        endwin();
        return 0;
}
    band = openFile(fileName); //fileio.cpp

    if ( band.maxidx == 0 ) {
        printw("Error! Please try again.\n");
        refresh();
        goto MAIN_MENU;
    }

    getyx(stdscr,row,col);
    chkend(row);    // See if there is room to print
    printw("\n\nPress ENTER when you would like to start. . . \n");
    refresh();
    while ( getch() != '\n' );

    recordStream(band); // Set-up and record!

    goto MAIN_MENU;     // I know, I know...

    return 0;   // By design this will never happen. Ctrl+C to quit.

}

/** Splash Screen for DEFCon301 **/
void splash() {

    int         R, C, i, j;

    R = getmaxy(stdscr);
    C = getmaxx(stdscr);
    /* Fill blank space */
    for (i=0; i<R; i++) {
        for (j=0; j<C; j++) {
            printw("#");
        }
    }
    move(R/4-2,0);

    printw(  "    *****     ****  ****  ****   ****      *    * \n"    );
    printw(  "   *    **   ***   ***   *      *    *    **   * \n"     );
    printw(  "  **    **  *     *     **      *    *   *   ** \n");
    printw(  " *******   ****  *       ****    ****   *    * \n");
    printw(  "                         ****  ****  *** \n" );
    printw(  "                          ***  *  *    *  \n");
    printw(  "                            *  *  *    * \n");
    printw(  "                        *****  ****  **** \n");
    refresh();

/*
    printw(  "      *****       ******   ******  ****    ****         *          * \n"    );
    printw(  "     **   **     *        *       *       *    *       ***        * \n"     );
    printw(  "    **    **    *        *       *       *      *     *  **      * \n"      );
    printw(  "   **     **   ******   *****   *       *        *   *    **    * \n"       );
    printw(  "  **     **   *        *         *       *      *   *      **  * \n"        );
    printw(  " **    **    *        *           *       *    *   *        *** \n"         );
    printw(  " *******    *******  *             ****    ****   *          ** \n\n"         );
    printw(  "                                                 ******* *******  **** \n " );
    printw(  "                                                      * *     *     *  \n");
    printw(  "                                                       * *     *     * \n");
    printw(  "                                                  ****** *     *     * \n");
    printw(  "                                                       * *     *     * \n");
    printw(  "                                                ******** ******* *******  \n");
    refresh();
*/
    return;

}
