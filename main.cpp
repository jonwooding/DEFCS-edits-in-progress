/** @file       main.cpp
    @brief      Director for DEFCon301
    @author     Jonathan Wooding, Lamar University, EE
    @version    1.02.14
*/

/** \todo       convert to mono <><>
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

void splash();

int main(void) {
    bandData        band;
    int             row, col;

    initscr();              // setup terminal screen
    cbreak();               // no need to press enter
    noecho();               // shhhh
    curs_set(0);            // invisible cursor for now
    keypad(stdscr, TRUE);    // arrow keys
    scrollok(stdscr, TRUE);

    splash();
    getch();
    erase();
    curs_set(2);

MAIN_MENU:
    band = mainMenu();      // get pointer to open preset after it is opened
    if ( band.maxidx == 0 ) {
        printw("Error! Please try again.\n");
        refresh();
        goto MAIN_MENU;
    }

    getyx(stdscr,row,col);
    chkend(row);
    printw("\n\nPress ENTER when you would like to start. . . \n");
    refresh();
    while ( getch() != '\n' ) ;

    recordStream(band);

    if ( band.maxidx != 0 ) {
        free( band.hind );
        free( band.lind );
        free( band.dmx );
        free( band.dmx_size );
        free( band.avg );
    }

    goto MAIN_MENU;

    return 0;

}

void splash() {

    int         R, C, i, j;

    R = getmaxy(stdscr);
    C = getmaxx(stdscr);

    for (i=0; i<R; i++) {
        for (j=0; j<C; j++) {
            printw("#");
        }
    }
    move(R/4-2,0);

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


}
