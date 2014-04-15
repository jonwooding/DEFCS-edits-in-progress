/** @file       stream.cpp
    @brief      Functionality of DEFCon301
    @author     Jonathan Wooding, Lamar University, EE
    @version    1.02.14
*/

#include <math.h>
#include "stream.h"

int recordStream(bandData band) {
    PaStreamParameters  inputParameters;
    PaStream*           stream;
    PaError             err = paNoError;
    paTestData          data;
    ola::DmxBuffer      buffer;
    int                 i, stopKey, totalFrames, numSamples,
                        numBytes, ind, diff, avg;
    int                 FLASH_THRESHOLD = 8;
    int                 in_threshold = 15;
    int                 in_max = 0;
    unsigned int        universe = 1;
    float               red = 0.85;
    float               intensity, maxx;

    timeout(0);         // to allow key-press interrupt

    fftData myData = setup();   // set up FFT / allocate memory (in fft_setup.cpp)


    /********************************************************************************/
    /************************Set up DMX server***************************************/
    /********************************************************************************/
    // universe to use for sending data
// turn on OLA logging
    ola::InitLogging(ola::OLA_LOG_WARN, ola::OLA_LOG_STDERR);

    buffer.Blackout(); // Set all channels to 0
// Create a new client.
    ola::StreamingClient ola_client((ola::StreamingClient::Options()));
// Setup the client, this connects to the server
    if (!ola_client.Setup()) {
        std::cerr << "Setup failed" << endl;
        exit(1);
    }


    /********************************************************************************/
    /************************Set up audio stream*************************************/
    /********************************************************************************/
    data.maxFrameIndex = totalFrames = FRAMES_PER_BUFFER; // Record 1 buffer
    data.frameIndex = 0;
    numSamples = totalFrames * NUM_CHANNELS;
    numBytes = numSamples * sizeof(SAMPLE);
    data.recordedSamples = (SAMPLE *) malloc( numBytes ); // where it will record

    if( data.recordedSamples == NULL ) {
        printf("Could not allocate record array.\n");
        goto done;
    }
    for( i=0; i<numSamples; i++ ) data.recordedSamples[i] = 0;

    err = Pa_Initialize();
    if( err != paNoError ) goto done;

    inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
    if (inputParameters.device == paNoDevice) {
        fprintf(stderr,"Error: No default input device.\n");
        goto done;
    }
    inputParameters.channelCount = 1;                    /* stereo input *//** trying to match NUM_CHANNELS*/
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    err = Pa_OpenStream(
              &stream,
              &inputParameters,
              NULL,                  /* &outputParameters, */
              SAMPLE_RATE,
              FRAMES_PER_BUFFER,
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              recordCallback,
              &data );
    if( err != paNoError ) goto done;

    /********************************************************************************/
    /***********Record buffer of audio, perform FFT, process, send*******************/
    /********************************************************************************/

//    FILE *pFile;
//    pFile = fopen("audio.csv","w");

    /**Set up static intensity channels*/
    for (i=0; i<band.num_int_channels; i++) {
        buffer.SetChannel(band.int_channels[i], 255);
    }
    ola_client.SendDmx(universe, buffer);
    /** Clear screen of any errors PA throws, create new instance of stdscr */
    erase();
    refresh();
    endwin();
    initscr();              // setup terminal screen
    cbreak();               // no need to press enter
    noecho();               // shhhh
    curs_set(0);            // invisible cursor for now
    keypad(stdscr, TRUE);    // arrow keys
    scrollok(stdscr, TRUE);
    curs_set(2);
    move(0,0);
    printw("\n\n\n\n\nDEFCon301 is running. . .\n\n\t\tPress any key to stop.");
    refresh();

    while (1) {
        in_max = in_threshold;
        /* Grab a buffer of audio */
        err = Pa_StartStream( stream );
        if( err != paNoError ) goto done;

        while( ( err = Pa_IsStreamActive( stream ) ) == 1 ) {
            // Wait for data
        }
        Pa_StopStream( stream );

        /* Create left and right array -- int to double */
        for ( i=0; i<data.maxFrameIndex; i++ ) { /** this is converting for mono too **/
            myData.in[i] = data.recordedSamples[i];
            if ( abs(myData.in[i]) > in_max ) {
                in_max = abs(myData.in[i]);
            }
        }
        if ( in_max > in_threshold ) {
            /* FFT myData.in, store in myData.out */
            fftwf_execute(myData.method);

            maxx = 0;
            for (i=3; i<FRAMES_PER_BUFFER; i++) { /** k=3 :: don't count sub 60 hz **/
                if ( abs(myData.out[i][0]) > maxx ) {
                    maxx = abs(myData.out[i][0]);
                }
            }

            for ( ind=0; ind<band.maxidx; ind++ ) {
                avg = 0;
                diff = band.hind[ind] - band.lind[ind];
                for ( i=band.lind[ind]; i<=band.hind[ind]; i++ ) {
                    avg += abs(myData.out[i][0]);
                }
                avg = avg / diff;       // avg value
                avg = avg * 255 / (maxx) * band.gain[ind];     // max to 255
                avg = avg;  /** This was divided by 2, why? */

                if ( avg > band.avg[ind] ) {
                    band.avg[ind] = avg;
                } else if (band.avg[ind] - avg < FLASH_THRESHOLD) {
                    // do nothing
                } else {
                    band.avg[ind] = band.avg[ind] * red; // Note: this is nonlinear reduction
                }

                intensity = floor( band.avg[ind] );

                if (intensity > 255) {
                    intensity = 255;    // Clip to max value
                } else if (intensity < 30) {
                    intensity = 0;
                } else if (intensity < 40) {
                    intensity -= 30;
                } else if (intensity < 50) {
                    intensity -= 20;
                } else if (intensity < 60) {
                    intensity -= 10;
                }

                for (i=0; i<=band.dmx_size[ind]; i++) {
                    buffer.SetChannel(band.dmx[ind][i], intensity);
                }
            }


            if (!ola_client.SendDmx(universe, buffer)) {
                printw("Problem sending DMX, will retry in 5. . .");
                refresh();
                sleep(5);
            }
            /** If no input is present */
        } else {
            buffer.Blackout();
            for (i=0; i<band.num_int_channels; i++) {
                buffer.SetChannel(band.int_channels[i], 255);
            }
            if (!ola_client.SendDmx(universe, buffer)) {
                printw("Problem sending DMX, will retry in 5. . .");
                refresh();
                sleep(5);
            }
        }

        /** for examining mono stream **/
/*
        timeout(-1);
        erase();
        for (i=1014; i<1034; i++) {

            printw("%i\t%f\n",i, myData.in[i]);
            refresh();

        }
        refresh();
        getch();
        timeout(0);
*/
        data.frameIndex = 0;

        stopKey = getch();
        if ( stopKey != ERR ) {
            break;
        }

    }

 //   fclose(pFile);

    timeout(-1);        // return to blocking mode

    buffer.Blackout();
    if (!ola_client.SendDmx(universe, buffer)) {
        cout << "Send DMX failed" << endl;
        exit(1);
    }

    /********************************************************************************/
    /*************************Close audio stream and free memory*********************/
    /********************************************************************************/
    err = Pa_CloseStream( stream );
    if( err != paNoError ) goto done;

done:
    Pa_Terminate();
    if( data.recordedSamples )       /* Sure it is NULL or valid. */
        free( data.recordedSamples );
    if( err != paNoError ) {
        fprintf( stderr, "An error occured while using the portaudio stream\n" );
        fprintf( stderr, "Error number: %d\n", err );
        fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
        err = 1;          /* Always return 0 or 1, but no other return codes. */
    }

    fftwf_free(myData.in);
    fftwf_free(myData.out);
    fftwf_destroy_plan(myData.method);
/*
    free( band.hind );
    free( band.lind );
    free( band.dmx );
    free( band.dmx_size );
    free( band.avg );
    free( band.gain );
    free( band.int_channels );
*/
    delete[] band.lind;
    delete[] band.hind;
    for (i=0; i<band.maxidx; i++){
        delete[] band.dmx[i];
    }
    delete[] band.dmx;
    delete[] band.dmx_size;
    delete[] band.avg;
    delete[] band.gain;
    delete[] band.int_channels;


    return 0;

}
