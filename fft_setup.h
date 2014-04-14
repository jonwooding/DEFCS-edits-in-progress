#include <fftw3.h>

typedef struct {
    double              *in;
    fftw_complex        *out;
    fftw_plan            method;
}
fftData;

fftData setup(void);
