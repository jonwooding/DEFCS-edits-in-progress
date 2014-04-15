/** @file       menu.cpp
    @brief      Main menu controller for DEFCon301
    @author     Jonathan Wooding, Lamar University, EE
    @version    1.02.14
*/

/** mainMenu() | arrows() **/

#include "menu.h"

bandData mainMenu() {
    int                 row, col, selection, k;
    int                 num_options;
    int                 *dirArray;
    bandData            band;
    string              fileName, point;
    vector<string>      dirlist;
    string              path = "./presets/";

    dirArray = (int*) malloc( 50 * sizeof(int) ); // To store dirlist locations (presets)
    scrollok(stdscr, TRUE);

MAIN_MENU:
    erase();
    printw("What would you like to do?\n");
    printw("\tUse arrow keys to decide.\n");

    getyx(stdscr,row,col);

    printw("1. Load preset\n");
    printw("2. Create new preset\n");
    num_options = 2;
    move(row,col);
    refresh();
    selection = arrows(num_options);    // UI Control function.
    erase();

    switch (selection) {
    case 1: // Load preset
        printw("\nWhich preset would you like to load?\n\n");
        getyx(stdscr,row,col);
        dirlist = listPresets();
        num_options = 0;
        /* Show contents of ./presets */
        for (k=0; k<dirlist.size(); k++) {
            point = dirlist[k].c_str();
            if ( point[0] != '.' ) {
                dirArray[num_options] = k;
                printw("%i. %s\n",num_options+1,dirlist[k].c_str());
                num_options++;
            }
        }
        move(row,col);
        refresh();
        selection = arrows(num_options)-1;
        fileName = path + dirlist[dirArray[selection]];
        band = openFile(fileName); //fileio.cpp
        refresh();
        break;

    case 2: // Create preset
        fileName = namePreset(); // typeName.cpp
        printw("\nCreating file: %s. . .\n\n",fileName.c_str());
        fileName = path + fileName;
        refresh();
        if ( newPreset(fileName) ) {
            printw("\n\nPreset succesfully created.  Press enter to return to main menu.");
            refresh();
            getch();
            goto MAIN_MENU;
        }
        break;
    }

    free( dirArray );

    return band;

}

int arrows(int num_options) {

    int         resp, row, col, minrow;

    getyx(stdscr,row,col); // Get current location
    minrow = row;

    while ( ( resp = getch() ) != (char)'\n' ) {
        switch (resp) {
        case '8': // UP
            row--;
            break;
        case '2': // DOWN
            row++;
            break;
        }

        if (row < minrow) {
            row = minrow;
        } else if (row > minrow + num_options - 1) {
            row = minrow + num_options - 1;
        }
        move(row,col);
        refresh();
    }

    return  (row - minrow + 1);

}
