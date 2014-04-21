/** @file       main.cpp
    @brief      Configuration for DEFCon301
    @author     Jonathan Wooding, Lamar University, EE
    @version    1.02.14
*/

#include <ncurses.h>

#if 0
#define KEY "numpad"
#define UP       '8'
#define DOWN        '2'
#define LEFT        '4'
#define RIGHT       '6'
#define SELECT      '1'
#define SELECT_STR  "'1'"
#define STOP        '0'
#define STOP_STR    "'0'"
#elif 1
#define KEY "arrow"
#define UP          KEY_UP
#define DOWN        KEY_DOWN
#define LEFT        KEY_LEFT
#define RIGHT       KEY_RIGHT
#define SELECT      UP
#define SELECT_STR  "UP"
#define STOP        DOWN
#define STOP_STR    "DOWN"
#endif // 1
