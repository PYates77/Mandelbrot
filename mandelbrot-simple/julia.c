#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <complex.h>
#include <math.h>

#define FILL "X"
#define NOFILL " "

#define X0 -2.0
#define Y0 -1.5
#define X1 2.0
#define Y1 1.5
#define XSTEPS 160
#define YSTEPS 80
#define XSTEP (X1-X0)/XSTEPS
#define YSTEP (Y1-Y0)/YSTEPS
#define THRESHOLD 2
#define ITERATIONS 100

// good result C = -0.328132 + 0.716981i

int main()
{
    double exp = 2;
    double angle = -0.01;
    //double complex c = 0.7885;
    double complex c = -0.02 - 0.79*I;
    while (1) {
        printf("C = %3f + %3fi\nexp=%f\nangle=%f\n", creal(c), cimag(c), exp, angle);
        complex cursor = X0 + Y0*I;
        for (int y = 0; y < YSTEPS; ++y) {
            for (int x = 0; x < XSTEPS; ++x) {
                complex z = cursor;
                //complex z = 0;
                int thresh = 0;
                for (int itr = 0; itr < ITERATIONS; ++itr) {
                    z = cpow(z,exp) + c;
                    if (cabs(z) > THRESHOLD) {
                        thresh = 1;
                        break;
                    }
                }
                //printf("%2f", cabs(z));
                if (thresh) {
                    printf(NOFILL);
                } else {
                    printf(FILL);
                }
                cursor += XSTEP;
            }
            cursor = X0 + (cimag(cursor) + YSTEP)*I;
            puts("");
        }
        //exp += 0.1;
	//angle += 0.1;
        c *= cos(angle) + sin(angle)*I;
	//break;
	sleep(1);
        //system("clear");
    }
}