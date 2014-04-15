/** @file       fft_setup.cpp
    @brief      Create FFT plan for repeated use in DEFCon301
    @author     Jonathan Wooding, Lamar University, EE
    @version    1.02.14
*/

#include "record.h"

fftData setup(void) {
    int             N = FRAMES_PER_BUFFER + INPUT_PADDING;//NUM_SECONDS * SAMPLE_RATE;    // buffer size
    float           *in;        // input array
    fftwf_complex   *out;       // output 2d array (float)
    fftData         myData;     // struct to hold it all

    // allocate memory
    in = (float*) fftwf_malloc( sizeof(float) * N);
    out = (fftwf_complex*) fftwf_malloc( sizeof(fftwf_complex) * N);

    // let our struct get the pointers

    for (int i=0; i<N; i++) { /* clear input array for zero padding */
        in[i] = 0;
    }
    myData.in   =   in;
    myData.out  =   out;
    // create the plan - DFT, real2complex, 1 dimensional input
    myData.method = fftwf_plan_dft_r2c_1d(N, myData.in, myData.out, FFTW_MEASURE);

    return myData;

}
