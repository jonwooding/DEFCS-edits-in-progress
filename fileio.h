#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <vector>
#include <ncurses.h>
#include "cstring"
#include <unistd.h>

#define GAIN_SCALE (50)

using namespace std;

typedef struct {
    int                 *hind, *lind, *int_channels, *dmx_size;
    int                 **dmx;
    int                 idx, maxidx, num_int_channels;
    double              *avg;
    float               *gain;
} bandData;


int newPreset(string presetName);
bandData openFile(string presetName);
int chkend(int row);
