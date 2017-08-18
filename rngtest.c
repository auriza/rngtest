/*
 * rngtest - random number generator test
 *
 * Written in 2017 by Auriza Akbar <auriza.akbar@gmail.com>
 */

#include <stdlib.h>
#include <math.h>
#include "rngtest.h"
#include "lib/chisq.c"

#define POW2(x) (1L << (x))
#define SQ(x)   ((x)*(x))

#define FIPS_N          20000
#define FIPS_N1_MIN     9654
#define FIPS_N1_MAX     10346
#define FIPS_POKER_M    4
#define FIPS_POKER_MIN  1.03
#define FIPS_POKER_MAX  57.4

//const char *status_str[] = {
    //"Test passed",
    //"Test failed",
    //"Error: m too big",
    //"Error: d too big"
//};

long pow10_(long x) {
    long n = 1, i;
    for (i = 0; i < x; i++)
        n *= 10;
    return n;
}

/* Five basic tests --------------------------------------------------------- */

test freq(bit *S, long N, double a) {
    long    n[2] = {0};
    long    i;
    double  X;
    status  st;

    for (i = 0; i < N; i++)
        n[S[i]]++;

    X  = (double) (SQ(n[0]-n[1])) / N;
    st = (X < critchi(a, 1)) ? PASS : FAIL;
    return (test) {X, st};
}

test serial(bit *S, long N, double a) {
    long    n[2]     = {0};
    long    nn[2][2] = {{0}};
    long    i;
    double  X;
    status  st;

    for (i = 0; i < N; i++)
        n[S[i]]++;
    for (i = 0; i < N-1; i++)
        nn[S[i]][S[i+1]]++;

    X  = 4.0/(N-1) * (SQ(nn[0][0]) + SQ(nn[0][1]) + SQ(nn[1][0]) + SQ(nn[1][1]))
           - 2.0/N * (SQ(n[0]) + SQ(n[1])) + 1;
    st = (X < critchi(a, 2)) ? PASS : FAIL;
    return (test) {X, st};
}

test poker(bit *S, long N, long m, double alpha) {
    long    k  = N/m;
    long    *n;
    long    n2_sum = 0;
    long    i, j, val;
    double  X;
    status  st;

    if (k < 5*POW2(m))
        return (test) {INFINITY, ERR_M2BIG};

    n = calloc(POW2(m), sizeof(long));
    for (i = 0; i < N-m+1; i += m) {
        val = 0;
        for (j = 0; j < m; j++)
            val += S[i+j] << (m-j-1);
        n[val]++;
    }
    for (i = 0; i < POW2(m); i++)
        n2_sum += SQ(n[i]);
    free(n);

    X  = (double) POW2(m)/k * n2_sum - k;
    st = (X < critchi(alpha, POW2(m)-1)) ? PASS : FAIL;
    return (test) {X, st};
}

test runs(bit *S, long N, double alpha) {
    long    k = log2(N/20.0);               /* max runs length to be considered (simplified) */
    double  *e = calloc(k, sizeof(double)); /* expected number of runs   */
    long    *B = calloc(k, sizeof(long));   /* observed number of blocks */
    long    *G = calloc(k, sizeof(long));   /* observed number of gaps   */
    long    i, prev, count;
    double  X = 0.0;
    status  st;

    for (i = 0; i < k; i++)
        e[i] = (double) (N-i+2) / POW2(i+3);

    prev  = S[0];
    count = 1;
    for (i = 1; i <= N; i++) {
        if (S[i] == prev && i < N)
            count++;
        else {
            if (count <= k) {
                if (prev == 1) B[count-1]++;
                          else G[count-1]++;
            }
            prev  = S[i];
            count = 1;
        }
    }

    for (i = 0; i < k; i++) {
        X += (SQ(B[i] - e[i])) / e[i];
        X += (SQ(G[i] - e[i])) / e[i];
    }
    free(e); free(B); free(G);
    st = (X < critchi(alpha, 2*k-2)) ? PASS : FAIL;
    return (test) {X, st};
}

test autocorr(bit *S, long N, long d, double alpha) {
    long    A = 0;
    long    i;
    double  X;
    status  st;

    if (d > N/2)
        return (test) {INFINITY, ERR_D2BIG};

    for (i = 0; i < N-d; i++)
        A += (S[i] ^ S[i+d]) ? 1 : 0;

    X = (double) 2 * (A - (N-d)/2) / sqrt(N-d);
    st = (X < critz(1 - alpha/2)) ? PASS : FAIL;
    return (test) {X, st};
}


/* Five basic tests (decimal) ----------------------------------------------- */

test freq_dec(bit *S, long N, double alpha) {
    long n[10] = {0};
    long n2_sum = 0;
    long i;
    double  X;
    status st;

    for (i = 0; i < N; i++)
        n[S[i]]++;

    for (i = 0; i < 10; i++)
        n2_sum += SQ(n[i]);

    X    = (double) 10/N * n2_sum - N;
    st = (X < critchi(alpha, 9)) ? PASS : FAIL;
    return (test) {X, st};
}

test poker_dec(bit *S, long N, long m, double alpha) {
    long    k  = N/m;
    long    *n;
    long    n2_sum = 0;
    long    i, j, val;
    double  X;
    status  st;

    if (k < 5*pow10_(m))
        return (test) {INFINITY, ERR_M2BIG};

    n = calloc(pow10_(m), sizeof(long));
    for (i = 0; i < N-m+1; i += m) {
        val = 0;
        for (j = 0; j < m; j++)
            val += S[i+j] * pow10_(m-j-1);
        n[val]++;
    }
    for (i = 0; i < pow10_(m); i++)
        n2_sum += SQ(n[i]);
    free(n);

    X  = (double) pow10_(m)/k * n2_sum - k;
    st = (X < critchi(alpha, pow10_(m)-1)) ? PASS : FAIL;
    return (test) {X, st};
}

test autocorr_dec(bit *S, long N, long d, double alpha) {
    long   n[10] = {0};
    double e;
    long   i, val;
    double X = 0.0;
    status st;

    if (d > N/2)
        return (test) {INFINITY, ERR_D2BIG};

    for (i = 0; i < N-d; i++) {
        val = (S[i] - S[i+d] + 10) % 10;
        n[val]++;
    }

    e = (double) (N-d)/10;
    for (i = 0; i < 10; i++)
        X += SQ(n[i]-e) / e;
    st = (X < critchi(alpha, 9)) ? PASS : FAIL;
    return (test) {X, st};
}

/* FIPS 140-1 tests --------------------------------------------------------- */

test fips_monobit(bit *S) {
    long    n1 = 0;
    long    i;
    double  X;
    status  st;

    for (i = 0; i < FIPS_N; i++)
        n1 += S[i];

    X  = (double) n1;
    st = (FIPS_N1_MIN < n1
                     && n1 < FIPS_N1_MAX) ? PASS : FAIL;
    return (test) {X, st};
}

test fips_poker(bit *S) {
    test   X;
    status st;

    X  = poker(S, FIPS_N, FIPS_POKER_M, NAN);
    st = (FIPS_POKER_MIN < X.val
                        && X.val < FIPS_POKER_MAX) ? PASS : FAIL;
    return (test) {X.val, st};
}
