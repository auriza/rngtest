/*
 * rngtest - random number generator test
 *
 * Written in 2017 by Auriza Akbar <auriza.akbar@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rngtest.h"
#include "lib/chisq.c"

#define pow2(x)         (1L << (x))
#define sq(x)           ((x)*(x))

long _pow(int base, int exp) {
    long n = 1, i;
    for (i = 0; i < exp; i++)
        n *= base;
    return n;
}

bit *read_sequence(char *filename, long *N) {
    bit *S;
    FILE *fp;
    long i, n;

    fp = fopen(filename, "r");
    fseek(fp, 0, SEEK_END);
    n = ftell(fp);
    rewind(fp);
    S = calloc(n+1, sizeof(bit));
    for (i = 0; !feof(fp); i++)
        fscanf(fp, "%1hhu", &S[i]);
    *N = i-1;
    fclose(fp);

    return S;
}

bit *fips_read_sequence(char *filename) {
    bit *S;
    FILE *fp;
    long i, n;

    fp = fopen(filename, "r");
    S = calloc(FIPS_N, sizeof(bit));
    for (i = 0; i < FIPS_N; i++)
        fscanf(fp, "%1hhu", &S[i]);
    fclose(fp);

    return S;
}

/* Five basic tests --------------------------------------------------------- */

test freq(bit *S, long N, double a) {
    long    n[2] = {0};
    long    i;
    double  X;
    status  st;

    for (i = 0; i < N; i++)
        n[S[i]]++;

    X  = (double) (sq(n[0]-n[1])) / N;
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

    X  = 4.0/(N-1) * (sq(nn[0][0]) + sq(nn[0][1]) + sq(nn[1][0]) + sq(nn[1][1]))
           - 2.0/N * (sq(n[0]) + sq(n[1])) + 1;
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

    if (k < 5*pow2(m))
        return (test) {INFINITY, ERR_M2BIG};

    n = calloc(pow2(m), sizeof(long));
    for (i = 0; i < N-m+1; i += m) {
        val = 0;
        for (j = 0; j < m; j++)
            val += S[i+j] << (m-j-1);
        n[val]++;
    }
    for (i = 0; i < pow2(m); i++)
        n2_sum += sq(n[i]);
    free(n);

    X  = (double) pow2(m)/k * n2_sum - k;
    st = (X < critchi(alpha, pow2(m)-1)) ? PASS : FAIL;
    return (test) {X, st};
}

test runs(bit *S, long N, double alpha) {
    long    k = log2(N/20.0);               // max runs length to be considered (simplified)
    double  *e = calloc(k, sizeof(double)); // expected number of runs
    long    *B = calloc(k, sizeof(long));   // observed number of blocks
    long    *G = calloc(k, sizeof(long));   // observed number of gaps
    long    i, prev, count;
    double  X = 0.0;
    status  st;

    for (i = 0; i < k; i++)
        e[i] = (double) (N-i+2) / pow2(i+3);

    prev  = S[0];
    count = 1;
    for (i = 1; i <= N; i++) {
        if (i < N && S[i] == prev)
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
        X += (sq(B[i] - e[i])) / e[i];
        X += (sq(G[i] - e[i])) / e[i];
    }
    free(e); free(B); free(G);
    st = (X < critchi(alpha, 2*k-2)) ? PASS : FAIL;
    return (test) {X, st};
}

test autocorr(bit *S, long N, long d, double alpha) {
    long    A = 0;
    long    i;
    double  Z;
    status  st;

    if (d > N/2)
        return (test) {INFINITY, ERR_D2BIG};

    for (i = 0; i < N-d; i++)
        A += (S[i] ^ S[i+d]) ? 1 : 0;

    Z  = (double) 2 * (A - (N-d)/2) / sqrt(N-d);
    st = (Z < critz(1 - alpha/2)) ? PASS : FAIL;
    return (test) {Z, st};
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
        n2_sum += sq(n[i]);

    X    = (double) 10/N * n2_sum - N;
    st = (X < critchi(alpha, 9)) ? PASS : FAIL;
    return (test) {X, st};
}

test serial_dec(bit *S, long N, double a) {
    long    n[10]      = {0};
    long    nn[10][10] = {{0}};
    long    n2_sum     = 0;
    long    nn2_sum    = 0;
    long    i, j;
    double  X;
    status  st;

    for (i = 0; i < N; i++)
        n[S[i]]++;
    for (i = 0; i < N-1; i++)
        nn[S[i]][S[i+1]]++;

    for (i = 0; i < 10; i++) {
        n2_sum += sq(n[i]);
        for (j = 0; j < 10; j++)
            nn2_sum += sq(nn[i][j]);
    }

    X  = 100.0/(N-1) * nn2_sum - 10.0/N * n2_sum + 1;
    st = (X < critchi(a, 90)) ? PASS : FAIL;
    return (test) {X, st};
}

test poker_dec(bit *S, long N, long m, double alpha) {
    long    k  = N/m;
    long    *n;
    long    n2_sum = 0;
    long    i, j, val;
    double  X;
    status  st;

    if (k < 5*_pow(10, m))
        return (test) {INFINITY, ERR_M2BIG};

    n = calloc(_pow(10, m), sizeof(long));
    for (i = 0; i < N-m+1; i += m) {
        val = 0;
        for (j = 0; j < m; j++)
            val += S[i+j] * _pow(10, m-j-1);
        n[val]++;
    }
    for (i = 0; i < _pow(10, m); i++)
        n2_sum += sq(n[i]);
    free(n);

    X  = (double) _pow(10, m)/k * n2_sum - k;
    st = (X < critchi(alpha, _pow(10, m)-1)) ? PASS : FAIL;
    return (test) {X, st};
}

test runs_dec(bit *S, long N, double alpha) {
    long   *n, i, j, sum = 0, prev, runlen, runs = 0;
    double mean, var, subs, Z, pval;
    status st;

    n = calloc(10, sizeof(long));

    prev = S[0];
    n[prev]++;
    runlen = 1;
    for (i = 1; i < N; i++) {
        n[S[i]]++;
        if (S[i] == prev)
            runlen++;
        if ((S[i] != prev) || (i == N-1)) {
            runs++;
            prev   = S[i];
            runlen = 1;
        }
    }

    mean = 1.0;
    var  = 0.0;
    subs = 0.0;

    for (i = 0; i < 10; i++) {
        var += n[i]/(double)N * (N - n[i]);
        for (j = 0; j < 10; j++) {
            if (j != i) {
                mean += 1.0 * n[i]/N*n[j];
                var  += 2.0 * n[i]/N*n[j]/(N-1)*(N-n[j]-1);
            }
        }
        var  += 2.0 * n[i]/N*(N-n[i])/(N-1)*(n[i]-1)*(N-n[i]-1);
        subs += 1.0 * n[i]/N*(N-n[i]);
    }
    subs *= subs;
    var  -= subs;
    var   = fabs(var);
    Z     = (runs - mean)/sqrt(fabs(var));
    pval  = poz(Z);
    st    = (Z < critz(1 - alpha/2)) ? PASS : FAIL;

#ifdef DEBUG
    printf("=======================================\n");
    printf("Runs test for decimal digits\n");
    printf("---------------------------------------\n");
    printf("Runs    = %lu\n",  runs);
    printf("Mean    = %.6f\n", mean);
    printf("STDEV   = %.6f\n", sqrt(var));
    printf("Z       = %.6f\n", Z);
    printf("P-value = %.6f\n", pval);
    printf("=======================================\n");
#endif

    return (test) {Z, st};
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
        X += sq(n[i]-e) / e;
    st = (X < critchi(alpha, 9)) ? PASS : FAIL;
    return (test) {X, st};
}

/* FIPS 140-1 tests --------------------------------------------------------- */

test fips_monobit(bit *S) {
    long    n1     = 0;
    long    n1_min = 9654;
    long    n1_max = 10346;
    long    i;
    double  X;
    status  st;

    for (i = 0; i < FIPS_N; i++)
        n1 += S[i];

    X  = (double) n1;
    st = (n1_min < n1 && n1 < n1_max) ? PASS : FAIL;
    return (test) {X, st};
}

test fips_poker(bit *S) {
    test   X;
    status st;
    double X_min = 1.03;
    double X_max = 57.4;

    X  = poker(S, FIPS_N, 4, INFINITY);
    st = (X_min < X.val && X.val < X_max) ? PASS : FAIL;
    return (test) {X.val, st};
}

test fips_runs(bit *S) {
    long    k  = 6;                         // max runs length to be considered
    long    *B = calloc(k, sizeof(long));   // observed number of blocks
    long    *G = calloc(k, sizeof(long));   // observed number of gaps
    long    i, prev, count;
    int     min[6] = {2267, 1079, 502, 223,  90,  90};  // required interval of runs
    int     max[6] = {2733, 1421, 748, 402, 223, 223};
    double  X  = 0.0;
    status  st = PASS;


    prev  = S[0];
    count = 1;
    for (i = 1; i <= FIPS_N; i++) {
        if (S[i] == prev && i < FIPS_N)
            count++;
        else {
            if (count > k) count = 6;
            if (prev == 1) B[count-1]++;
                      else G[count-1]++;
            prev  = S[i];
            count = 1;
        }
    }

    for (i = 0; i < k; i++) {
        if (B[i] < min[i] || B[i] > max[i]) st = FAIL; else X += 1.0;
        if (G[i] < min[i] || G[i] > max[i]) st = FAIL; else X += 1.0;
    }
    free(B); free(G);
    return (test) {X, st};
}

test fips_longrun(bit *S) {
    long    i, prev, count, maxrun = 0;
    double  X;
    status  st;

    prev  = S[0];
    count = 1;
    for (i = 1; i <= FIPS_N; i++) {
        if (S[i] == prev && i < FIPS_N)
            count++;
        else {
            if (count > maxrun) maxrun = count;
            prev   = S[i];
            count  = 1;
        }
    }

    st = (maxrun < 34) ? PASS : FAIL;
    X  = (double) maxrun;
    return (test) {X, st};
}
