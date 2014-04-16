/** @file       fileio.cpp
    @brief      Preset handling for DEFCon301
    @author     Jonathan Wooding, Lamar University, EE
    @version    1.02.14
*/

#include "fileio.h"
#include <math.h>
#include "record.h"

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
        while ( ( curr = getch() ) != '\n' ) {
            printw("%c",curr);
            refresh();
            input[k] = curr;
            k++;
        }
        lowFreq = atoi(input);
        memset(input, '\0', 5);
        printw("\nEnter higher cutoff frequency: ");
        refresh();
        k = 0;
        while ( ( curr = getch() ) != '\n' ) {
            printw("%c",curr);
            refresh();
            input[k] = curr;
            k++;
        }
        hiFreq = atoi(input);
        memset(input, '\0', 5);
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
        i++;
DMX_LOOP:
        printw("\nEnter DMX channel %i: ",i);
        refresh();
        while   ( ( ( curr = getch() ) != '0' ) ) {
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
            i++;
            printw("\nEnter DMX channel %i: ",i);
            refresh();
        }
        num_channels += i-1;
        printw("\nThe gain controls how much the bandwidth will affect the lights.");
        printw("\nGain values can be between -50 and 50.");
        printw("\nEnter gain for this band: 1");
        getyx(stdscr, row, col);
        move(row, col-1);
        refresh();
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

        // Check for continue
        printw("\nPress '0' to save and exit, press '1' to add another band. . .");
        refresh();
        if ( ( curr = getch() ) == '0' ) break;
        n++;
        k = 0;
    }

    printw("\n\nSome types of LED lights have an intensity channel\nthat will need to be turned on.");
    printw("\nEnter the address of any necessary intensity channels.\n");
    printw("\t\t\t( PRESS '0' TO STOP )\n");
    refresh();
    while ( ( curr = getch() ) != '0' ) {
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

    return 1;
}

bandData openFile(string presetName) {
    ifstream                preset;
    string                  line;
    vector<int>             int_channels;
    unsigned int            num_bands, num_channels, response,
             row, col, k, ind;
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
    } else {
        printw("Unable to open file.\n");
        return band;
    }
    refresh();
    preset.close();
    getyx(stdscr,row,col);
    chkend(row);
    printw("\n\nPress '1' to load this preset.");
    refresh();
    response = getch();
    printw("%c",response);
    refresh();
    if ( response != '1' ) {
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


    band.idx = 0;
    band.maxidx = num_bands;
    band.lind = new int[num_bands-1];
    band.hind = new int[num_bands-1];
    band.dmx_size = new int[num_bands];
    band.dmx = new int*[num_bands-1];
    for (int i=0; i<num_bands; i++ ){
        band.dmx[i] = new int[10];
    }
    band.avg = new float[num_bands-1];
    band.gain = new float[num_bands-1];
    band.int_channels = new int[int_channels.size()];
    for (k=0; k<int_channels.size(); k++) {
        band.int_channels[k] = int_channels[k]-1;
    }
    k = 1;
    band.num_int_channels = int_channels.size();

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
                        for (ind=1; ind<line.length(); ind++) {
                            temparray[ind-1] = line[ind];
                        }
                    }
                    temp.push_back(atoi(line.c_str()));
                    i++;
                }
                i--;
                i--; // for G selection
                preset.seekg(pos);
                printw("Retrieved %i channels. . .\n",i+1);
                refresh();
                usleep(55000);
                band.gain[k-1] = 1 + atof(temparray) / GAIN_SCALE;
                //band.dmx[k-1] = new int[i];
                for (n=0; n<=i; n++) {
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
