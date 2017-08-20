#include <stdio.h>
#include <stdlib.h>
#include "rngtest.h"

int main(int argc, char *argv[]) {
    bit    *s;                      // random binary sequence
    long   n;                       // length
    double alpha = 0.05;
    int    i;

    /// Menezes
    //n = 160;
    //s = read_sequence("data/basic.txt", n);
    //test X[5];
    //X[0] = freq(s, n, alpha);
    //X[1] = serial(s, n, alpha);
    //X[2] = poker(s, n, 3, alpha);
    //X[3] = runs(s, n, alpha);
    //X[4] = autocorr(s, n, 8, alpha);
    //for (i = 0; i < 5; i++)
        //printf("X%d = %.4f\t%s\n", i+1, X[i].val, status_str[X[i].stat]);

    /// Decimal
    n = 160000;
    s = read_sequence("data/des1.txt", n);
    test X2 = serial_dec(s, n, alpha);
    test X3 = poker_dec(s, n, 3, alpha);
    test X5 = autocorr_dec(s, n, 8, alpha);
    printf("X2 = %8g\t%s\n", X2.val, status_str[X2.stat]);
    printf("X3 = %8g\t%s\n", X3.val, status_str[X3.stat]);
    printf("X5 = %8g\t%s\n", X5.val, status_str[X5.stat]);

    /// FIPS
    //s = read_sequence("data/e.txt", FIPS_N);
    //test F1 = fips_monobit(s);
    //test F2 = fips_poker(s);
    //test F3 = fips_runs(s);
    //test F4 = fips_longrun(s);
    //printf("F1 = %8g\t%s\n", F1.val, status_str[F1.stat]);
    //printf("F2 = %8g\t%s\n", F2.val, status_str[F2.stat]);
    //printf("F3 = %8g\t%s\n", F3.val, status_str[F3.stat]);
    //printf("F4 = %8g\t%s\n", F4.val, status_str[F4.stat]);


    return 0;
}
