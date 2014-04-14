#include "typeName.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

std::string namePreset(void);

std::string namePreset() {
    int         row, col, mincol, ch;
    int         sh = 65;
    int         nums = 0;

    printw("Use the number pad arrows to enter a name.\n\n");
    printw("'8' / '2' == 'UP' / 'DOWN'\t|\t'6' / '4' == NEXT / PREVIOUS ENTRY\n\n");
    printw("Enter a name for the preset: ");

    getyx(stdscr,row,col);  // get cursor loc
    mincol = col;           // initialize far left
    mvaddch(row,col,sh);    // print letter
    move(row,col);          // keep cursor there
    refresh();
    while ( ( ch = getch() ) != (char)'\n' ) {
        switch (ch) {
        case '8':   // UP
            sh++;
            mvaddch(row,col,sh);
            break;
        case '2':   // DOWN
            sh--;
            mvaddch(row,col,sh);
            break;
        case '6':   // RIGHT
            col++;
            sh = 65;
            mvaddch(row,col,sh);
            break;
        case '4':   // LEFT
            if ( col == mincol ) {
                break;
            }
            col--;
            delch();
            break;

        }
        // Don't allow cursor beyond mincol
        if ( col < mincol ) {
            col = mincol;
        }
        // Check for Num->Letter wrapping
        if ( ( sh < 65 ) && !nums ) {
            nums = 1;
            sh = 57;
            mvaddch(row,col,sh);
        } else if ( ( sh < 48 ) && nums ) {
            nums = 0;
            sh = 90;
            mvaddch(row,col,sh);
        } else if ( sh > 90 ) {
            sh = 48;
            nums = 1;
            mvaddch(row,col,sh);
        } else if ( ( sh > 57 ) && ( sh < 65 ) ) {
            sh = 65;
            nums = 0;
            mvaddch(row,col,sh);
        }
        // Refresh with cursor in correct
        move(row,col);
        refresh();

    }

    move(row,mincol);

    char            *str = (char*) malloc(sizeof(char)*50);
    int             i = 0;
    int             k = 0;
    instr(str);                 // get char array
    while ( str[i] != ' ' ) i++;

    char            *newstr = (char*) malloc(sizeof(char)*50);

    while ( k <= i ) {
        newstr[k] = str[k];
        k++;
    }
    newstr[k-1] = '\0';
    std::string name(newstr);    // create string
    name += ".txt";         // conc .txt

    return name;
}
