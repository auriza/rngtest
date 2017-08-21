#include <stdio.h>
#include <stdlib.h>
#include "rngtest.h"

int main(int argc, char *argv[]) {
    bit    *s;                      // random binary sequence
    long   n;                       // length
    double alpha = 0.05;
    int    i;

    /// Menezes
    s = read_sequence("data/basic.txt", &n);
    printf("Menezes, n = %ld\n", n);
    test X[5];
    X[0] = freq(s, n, alpha);
    X[1] = serial(s, n, alpha);
    X[2] = poker(s, n, 3, alpha);
    X[3] = runs(s, n, alpha);
    X[4] = autocorr(s, n, 8, alpha);
    for (i = 0; i < 5; i++)
        printf("X%d = %10g\t%s\n", i+1, X[i].val, status_str[X[i].stat]);
    free(s);

    /// Decimal
    s = read_sequence("data/randomDec.txt", &n);
    printf("Decimal, n = %ld\n", n);
    test Y[5];
    Y[0] = freq_dec(s, n, alpha);
    Y[1] = serial_dec(s, n, alpha);
    Y[2] = poker_dec(s, n, 3, alpha);
    Y[3] = runs_dec(s, n, alpha);
    Y[4] = autocorr_dec(s, n, 8, alpha);
    for (i = 0; i < 5; i++)
        printf("Y%d = %10g\t%s\n", i+1, Y[i].val, status_str[Y[i].stat]);
    free(s);

    /// FIPS
    s = fips_read_sequence("data/e.txt");
    printf("FIPS\n");
    test F[4];
    F[0] = fips_monobit(s);
    F[1] = fips_poker(s);
    F[2] = fips_runs(s);
    F[3] = fips_longrun(s);
    for (i = 0; i < 4; i++)
        printf("F%d = %10g\t%s\n", i+1, F[i].val, status_str[F[i].stat]);
    free(s);

    return 0;
}
