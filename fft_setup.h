#include <fftw3.h>

typedef struct {
    float               *in;
    fftwf_complex       *out;
    fftwf_plan           method;
}
fftData;

fftData setup(void);
