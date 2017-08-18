#include <stdio.h>
#include <stdlib.h>
#include "rngtest.h"

int main(int argc, char *argv[]) {
    bit *s;                      // binary sequence
    long n;                      // length
    long i;
    FILE *in;
    double alpha = 0.05;         // significance level
    long m = 3;                  // poker test: length of subsequence
    long d = 8;                  // autocorr test: displacement shift

    // read sequence
    n  = 160;
    s  = calloc(n, sizeof(bit));
    in = fopen("data/basic.txt", "r");
    for (i = 0; i < n; i++)
        fscanf(in, "%1hhu", &s[i]);
    fclose(in);

    test X[5];
    X[0] = freq(s, n, alpha);
    X[1] = serial(s, n, alpha);
    X[2] = poker(s, n, m, alpha);
    X[3] = runs(s, n, alpha);
    X[4] = autocorr(s, n, d, alpha);

    for (i = 0; i < 5; i++)
        printf("X%d = %.4f\t%s\n", i+1, X[i].val, status_str[X[i].stat]);

    return 0;
}
