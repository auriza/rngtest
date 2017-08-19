/*
 * rngtest - random number generator test
 *
 * Written in 2017 by Auriza Akbar <auriza.akbar@gmail.com>
 */

#ifndef RNGTEST_H_INCLUDED
#define RNGTEST_H_INCLUDED

typedef unsigned char bit;

typedef enum status {
    PASS,
    FAIL,
    ERR_M2BIG,
    ERR_D2BIG
} status;

static const char* status_str[] = {
    "Passed",
    "Failed",
    "Error: m too big",
    "Error: d too big",
};

typedef struct test {
    double val;
    status stat;
} test;

bit *read_sequence(char *filename, long N);

/* Five basic tests (Menezes et al. 1996. Handbook of Applied Cryptography. pp 181-183) */
test freq     (bit *S, long N, double alpha);
test serial   (bit *S, long N, double alpha);
test poker    (bit *S, long N, long m, double alpha);
test runs     (bit *S, long N, double alpha);
test autocorr (bit *S, long N, long d, double alpha);

/* Five basic tests (decimal digit) */
test freq_dec    (bit *S, long N, double alpha);
//test serial_dec  (bit *S, long N, double alpha);
//test poker_dec   (bit *S, long N, long m, double alpha);
//test runs_dec    (bit *S, long N, double alpha);
test autocorr_dec(bit *S, long N, long d, double alpha);

/* FIPS 140-1 tests */
test fips_monobit (bit *S);
test fips_poker   (bit *S);
//test fips_runs    (bit *S);
//test fips_longrun (bit *S);

/* FIPS 140-2 tests */
//test fips2_monobit(bit *S);
//test fips2_poker  (bit *S);
//test fips2_runs   (bit *S);
//test fips2_longrun(bit *S);

/* Monkey tests */

#endif
