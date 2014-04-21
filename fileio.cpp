/** @file       fileio.cpp
    @brief      Preset handling for DEFCon301
    @author     Jonathan Wooding, Lamar University, EE
    @version    1.02.14
*/

#include "fileio.h"
#include <math.h>
#include "record.h"
#include "system_config.h"

int newPreset(string presetName) {
    ofstream                preset, copier;
    ifstream                precopy;
    string                  line;
    vector<int>             int_channels;
    char                    input[5];
    char                    curr;
    int                     response, lowFreq, hiFreq;
    unsigned int            k = 0;
    unsigned int            ind, i, row, col;
    unsigned int            n = 1;
    unsigned int            num_channels = 0;

    if ( KEY == "numpad" ){
        curs_set(2);
    } else if ( KEY == "arrow" ) {
        curs_set(0);
    }

    // Open the preset
    preset.open(presetName.c_str());
    preset << "NAME\t\t\t" << presetName << endl;

    printw("\n\nBegin by entering the bandwidth that you would like to control your lights.");
    printw("\nMore bandwidths can be entered later.\n");

    while (1) {
        // Begin gathering user data
getFreq:
        printw("\nEnter lower cutoff frequency: ");
        refresh();
        if ( KEY == "numpad" ) {
            k = 0;
            while ( ( curr = getch() ) != '\n' ) {
                printw("%c",curr);
                refresh();
                input[k] = curr;
                k++;
            }
            lowFreq = atoi(input);
            memset(input, '\0', 5);
        } else if ( KEY == "arrow" ) {
            int freq = 20;
            getyx(stdscr,row,col);
            move(row,col);
            printw("%i   ", freq);
            while ( ( response = getch() ) != '\n' ) {
                switch (response) {
                case UP:
                    freq += 20;
                    break;
                case DOWN:
                    freq -= 20;
                    break;
                case RIGHT:
                    freq += 100;
                    break;
                case LEFT:
                    freq -= 100;
                    break;
                }
                if (freq<20) freq=20;
                move(row,col);
                printw("%i    ", freq);
                refresh();
                lowFreq = freq;
            }
        }
        printw("\nEnter higher cutoff frequency: ");
        refresh();
        if ( KEY == "numpad" ) {
            k = 0;
            while ( ( curr = getch() ) != '\n' ) {
                printw("%c",curr);
                refresh();
                input[k] = curr;
                k++;
            }
            hiFreq = atoi(input);
            memset(input, '\0', 5);
        } else if ( KEY == "arrow" ) {
            int freq = lowFreq;
            getyx(stdscr,row,col);
            move(row,col);
            printw("%i", freq);
            while ( ( response = getch() ) != '\n' ) {
                switch (response) {
                case UP:
                    freq += 20;
                    break;
                case DOWN:
                    freq -= 20;
                    break;
                case RIGHT:
                    freq += 100;
                    break;
                case LEFT:
                    freq -= 100;
                    break;
                }
                if (freq<20) freq=20;
                move(row,col);
                printw("%i    ", freq);
                refresh();
                hiFreq = freq;
            }
        }

        // Check for valid data, repeat if invalid
        if ( (lowFreq > hiFreq) || (hiFreq > SAMPLE_RATE/2) || (lowFreq < 0) ) {
            printw("\nInvalid input, please try again.\n");
            refresh();
            goto getFreq;
        }

        preset << "BAND" << n << endl << lowFreq << endl << hiFreq << endl;
        preset << "DMX" << n << endl;

        // Get DMX channels
        i = 1;
        printw("Now enter the specific address of the light(s) that you'd\nlike to be controlled by this bandwidth");
        printw("\n=========== Press '0' when you're done adding channels ===========\n\n");
        printw("Enter DMX channel %i: ",i);
        refresh();
        if ( KEY == "numpad" ) {
            k = 0;
            while ( ( curr = getch() ) != '\n' ) {
                printw("%c",curr);
                refresh();
                input[k] = curr;
                k++;
            }
            response = atoi(input);
            memset(input, '\0', 5);
            preset << response << endl;
        } else if ( KEY == "arrow" ) {
            int dmx = 1;
            getyx(stdscr,row,col);
            move(row,col);
            printw("%i    ",dmx);
            refresh();
            while ( ( response = getch() ) != '\n' ) {
                switch (response) {
                case UP:
                    dmx++;
                    break;
                case DOWN:
                    dmx--;
                    break;
                case RIGHT:
                    dmx += 10;
                    break;
                case LEFT:
                    dmx -= 10;
                    break;
                }
                if (dmx<1) dmx=1;
                if (dmx>512) dmx=512;
                move(row,col);
                printw("%i    ", dmx);
                refresh();
                preset << dmx << endl;
            }
        }

        i++;
DMX_LOOP:
        printw("\nEnter DMX channel %i: ",i);
        refresh();
        while   ( ( ( curr = getch() ) != (char)STOP ) ) {
            if ( KEY == "numpad" ) {
                k = 0;
                do  {
                    printw("%c",curr);
                    refresh();
                    input[k] = curr;
                    k++;
                } while ( ( curr = getch() ) != '\n' );
                response = atoi(input);
                if ( response > 512 || response < 0 ) {
                    printw("\nInvalid input, try again\n");
                    memset(input, '\0', 5);
                    goto DMX_LOOP;
                }
                memset(input, '\0', 5);
                preset << response << endl;
            } else if ( KEY == "arrow" ) {
                int dmx = 1;
                getyx(stdscr,row,col);
                move(row,col);
                printw("%i    ",dmx);
                refresh();
                while ( ( response = getch() ) != '\n' ) {
                    switch (response) {
                    case UP:
                        dmx++;
                        break;
                    case DOWN:
                        dmx--;
                        break;
                    case RIGHT:
                        dmx += 10;
                        break;
                    case LEFT:
                        dmx -= 10;
                        break;
                    }
                    if (dmx<1) dmx=1;
                    if (dmx>512) dmx=512;
                    move(row,col);
                    printw("%i    ", dmx);
                    refresh();
                }
                preset << dmx << endl;
                i++;
                printw("\nEnter DMX channel %i: ",i);
                refresh();
            }
        }
        num_channels += i-1;
        printw("\nThe gain controls how much the bandwidth will affect the lights.");
        printw("\nGain values can be between -50 and 50.");
        printw("\nEnter gain for this band: 1");
        getyx(stdscr, row, col);
        move(row, col-1);
        refresh();
        if ( KEY == "numpad" ) {
            input[0]  = '1';
            k = 0;
            while ( ( curr = getch() ) != '\n' ) {
                printw("%c",curr);
                refresh();
                input[k] = curr;
                k++;
            }
            response = atof(input);
            if (abs(response) > 50) {
                response = 1;
                printw("\nOut of range. Using default [1].\n");
                refresh();
            }
            memset(input, '\0', 5);
            preset << "G" << response << endl;
        } else if ( KEY == "arrow" ) {
            int resp = 0;
            int gain = 1;
            getyx(stdscr,row,col);
            move(row,col);
            printw("%i    ", gain);
            refresh();
            while ( (resp=getch()) != (char)'\n' ) {
                switch (resp) {
                case UP:
                    gain++;
                    if (gain>=GAIN_SCALE) gain = GAIN_SCALE;
                    break;
                case DOWN:
                    gain--;
                    if (gain<=-GAIN_SCALE) gain = -GAIN_SCALE;
                    break;

                }
                move (row,col);
                printw("%i   ", gain);
                refresh();
            }
            preset << "G" << gain << endl;
        }

        // Check for continue
        printw("\nPress %s to save and exit, press %s to add another band. . .", STOP_STR, SELECT_STR);
        refresh();
        if ( ( curr = getch() ) == (char)STOP ) break;
        n++;
        k = 0;
    }

    printw("\n\nSome types of LED lights have an intensity channel\nthat will need to be turned on.");
    printw("\nEnter the address of any necessary intensity channels.\n");
    printw("\t\t\t( Press %s to stop )\n", STOP_STR);
    refresh();
    if ( KEY == "numpad" ) {
        while ( ( curr = getch() ) != (char)STOP ) {

            printw("%c",curr);
            refresh();
            k = 0;
            input[k] = curr;
            k = 1;
            while ( ( curr = getch() ) != '\n' ) {
                printw("%c",curr);
                refresh();
                input[k] = curr;
                k++;
            }
            printw("\n");
            int_channels.push_back(atof(input));
            memset(input, '\0', 5);
        }
    } else if ( KEY == "arrow" ) {
        int resp = 0;
        int nt = 1;
        getyx(stdscr,row,col);
        move(row,col);
        printw("%i   ", nt);
        refresh();
        while ( (resp = getch()) != STOP ) {
            do {
                switch (resp) {
                case UP:
                    nt += 1;
                    break;
                case DOWN:
                    nt -= 1;
                    break;
                case LEFT:
                    nt -= 10;
                case RIGHT:
                    nt += 10;
                }
                move(row,col);
                printw("%i    ",nt);
                refresh();

            } while ((resp = getch()) != '\n');
            move(row,col);
            printw("%i\n",nt);
            refresh();
            int_channels.push_back(nt);
        }
    }

    preset.close();
    // Copy file to include num bands/channels
    precopy.open(presetName.c_str());
    copier.open("temp.txt");
    getline(precopy,line);
    copier << line << endl;
    copier << "NUM_BANDS\n" << n << "\nNUM_CHANNELS\n" << num_channels << "\n";
    copier << "INTENSITY_CHANNELS\n";
    for (ind=0; ind<int_channels.size(); ind++) {
        copier << int_channels[ind] << "\n";
    }
    while (getline(precopy,line)) {
        copier << line << endl;
    }
    copier.close();
    precopy.close();
    remove(presetName.c_str());
    rename("temp.txt",presetName.c_str());
    remove("temp.txt");

    curs_set(2);

    return 1;
}

bandData openFile(string presetName) {
    ifstream                preset;
    string                  line;
    int                     num_bands, num_channels,
                            row, col, k, ind;
    int                     response;
    bandData                band;

    scrollok(stdscr, FALSE);

    band.maxidx = 0;

    // Get preset filename and open
    while(!preset.is_open()) {
        erase();
        printw("Preset filename: %s\n",presetName.c_str());
        refresh();
        preset.open(presetName.c_str());
    }

    // Display contents and close
    if (preset.is_open()) {
        vector<int>     int_channels;
        while ( getline (preset,line) ) {
            if ( line == "INTENSITY_CHANNELS") {
                //int_channels line
                printw("%s\n",line.c_str());
                while ( getline(preset,line) && line != "BAND1" ) {
                    int_channels.push_back(atoi(line.c_str()));
                    printw("%s\n",line.c_str());
                }
            }
            if ( line == "NUM_BANDS" ) {
                printw("%s\n",line.c_str());
                getyx(stdscr,row,col);
                chkend(row);
                getline(preset,line);
                printw("%s\n",line.c_str());
                getyx(stdscr,row,col);
                chkend(row);
                num_bands = atoi(line.c_str());
                getline(preset,line);
                printw("%s\n",line.c_str());
                getyx(stdscr,row,col);
                chkend(row);
                getline(preset,line);
                num_channels = atoi(line.c_str());
                getyx(stdscr,row,col);
                chkend(row);
            }
            printw("%s\n",line.c_str());
            getyx(stdscr,row,col);
            chkend(row);
        }

        band.int_channels = new int[int_channels.size()];
        for (k=0; k<int_channels.size(); k++) {
            band.int_channels[k] = int_channels[k]-1;
        }
        band.num_int_channels = int_channels.size();

    } else {
        printw("Unable to open file.\n");
        return band;
    }
    refresh();
    preset.close();
    getyx(stdscr,row,col);
    chkend(row);
    printw("\n\nPress %s to load this preset.", SELECT_STR);
    refresh();
    response = getch();
    if ( response != SELECT ) {
        printw("\nReturning to main menu.\n");
        refresh();
        sleep(2);
        return band;
    }


    /*
        band.idx = 0;
        band.maxidx = num_bands;
        band.lind = (int*) malloc( (num_bands - 1) * sizeof(int) );
        band.hind = (int*) malloc( (num_bands - 1) * sizeof(int) );
        band.dmx_size = (int*) malloc( num_bands * sizeof(int) );
        band.dmx = (int**) malloc( (num_bands - 1) * sizeof(int*) );
        band.avg = (double*) malloc( (num_bands - 1) * sizeof(double) );
        band.gain = (float*) malloc( (num_bands - 1) * sizeof(float) );
        band.int_channels = (int*) malloc( int_channels.size() * sizeof(int) );
        for (k=0; k<int_channels.size(); k++) {
            band.int_channels[k] = int_channels[k]-1;
        }
        k = 1;
        band.num_int_channels = int_channels.size();
    */

    k = 1;
    band.idx = 0;
    band.maxidx = num_bands;
    band.lind = new int[num_bands-1];
    band.hind = new int[num_bands-1];
    band.dmx_size = new int[num_bands];
    band.dmx = new int*[num_bands-1];
    for (int i=0; i<num_bands; i++ ) {
        //      band.dmx[i] = new int[10];
    }
    band.avg = new float[num_bands-1];
    band.gain = new float[num_bands-1];
    preset.open(presetName.c_str());
    printw("\nLoading preset. . . \n");
    refresh();

    if ( preset.is_open() ) {
        stringstream    ss;
        string          bandline = "BAND";
        vector<int>             temp;
        int             i, n, pos;
        float           skip;
        int lind, hind;
        skip = SAMPLE_RATE / 2;
        skip = skip / ( (FRAMES_PER_BUFFER + INPUT_PADDING)/2 - 1 );
        while ( k <= num_bands && getline(preset,line)) {
            i = 0;
            bandline = "BAND";
            ss.str(std::string());
            ss << k;
            bandline += ss.str();
            getyx(stdscr,row,col);
            chkend(row);
            if ( line == bandline ) {
                printw("Retrieving band %i. . .\n",k);
                refresh();
                usleep(55000);
                getline(preset,line);
                lind = round( atoi(line.c_str()) / skip );
                band.lind[k-1] = round( atoi(line.c_str()) / skip );
                getline(preset,line);
                band.hind[k-1] = round( atoi(line.c_str()) / skip );
                hind = round( atoi(line.c_str()) / skip );
                printw("Getting DMX channels for band %i\n",k);
                refresh();
                usleep(55000);
                ss.str(std::string());
                bandline = "BAND";
                ss << k+1;
                bandline += ss.str();
                getyx(stdscr,row,col);
                chkend(row);
                getline(preset,line);
                char    temparray[4];
                while( getline(preset,line) && ( line != bandline ) ) {
                    pos = preset.tellg();
                    if ( line[0] == 'G' ) {
                        for (ind=1; ind<=line.length(); ind++) {
                            temparray[ind-1] = line[ind];
                        }
                    }
                    temp.push_back(atoi(line.c_str()));
                    i++;
                }
                i--;
                preset.seekg(pos);
                printw("Retrieved %i channels. . .\n",i);
                refresh();
                usleep(55000);
                band.gain[k-1] = 1 + atof(temparray) / GAIN_SCALE;
                band.dmx[k-1] = new int[i];
                for (n=0; n<i; n++) {
                    band.dmx[k-1][n] = temp[n]-1;
                }
                band.dmx_size[k-1] = i;
                temp.clear();
                //i=0;
                k++;
            }
        }
        preset.close();
    } else {
        printw("Error loading preset.\n");
    }
    printw("Preset loaded.\n");
    scrollok(stdscr, TRUE);
    refresh();

    return band;

}

int chkend(int row) {
    int         maxrow;

    refresh();
    maxrow = getmaxy(stdscr);

    if (row > maxrow - 3) {
        printw("\t\tPress enter to scroll. . .");
        refresh();
        getch();
        erase();
        refresh();
        return 1;
    }

    return 0;

}
