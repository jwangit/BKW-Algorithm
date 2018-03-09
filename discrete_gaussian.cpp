#include <cmath>
#include <iostream>

#include <NTL/ZZ.h>

#include "discrete_gaussian.hpp"

using namespace std;
using namespace NTL;

/*
 * Initializes a discrete Gaussian generator with the provided standard deviation sigma and
 * number of standard deviations m.
 */
discrete_gaussian::discrete_gaussian(double sigma, long m) : sigma(sigma), m(m) {
    // Initialize constants.
    S = sigma * sqrt(2 * M_PI);
    max = (long) ceil(m * sigma);

    // Compute the probabilities for each x in [0,ceil(m*sigma)].
    probs = (double *) calloc(max, sizeof(double));
    probs[0] = prob(0) / 2; // Use half the probability of 0, to account for +0 and -0.
    for (long x = 1; x < max; x++) {
        probs[x] = prob(x);
    }
}

/*
 * Computes the probability of x according to this distribution. This is given exactly
 * by (1/S)*e^(-x^2 / 2*sigma^2), where S is a normalization factor, and we use the 
 * approximation S = sigma * sqrt(2 pi).
 */
double discrete_gaussian::prob(long x) {
    double x_real = conv<double>(x);
    return (1 / S) * exp(-(x_real * x_real) / (2 * sigma * sigma));
}

/*
 * Computes PP(a <= x <= b) according to this distribution by summing up P(x = i) for i
 * in [a,b].
 */
double discrete_gaussian::prob(long a, long b) {
    double result = 0;
    for (long x = a; x <= b; x++) {
        result += prob(x);
    }
    return result;
}

/*
 * Return the i-th bit (where 0 is the LEFTMOST bit) of the floating point representation of d.
 */
int get_bit(double d, int i) {
    uint8_t *tmp = (uint8_t *) malloc(sizeof(double));
    memcpy(tmp, &d, sizeof(double));
    int result = ((tmp[7 - i / 8] >> (i % 8)) & 1);
    free(tmp);
    tmp = NULL;
    return result;
}

/*
 * Generates a random number according to this distribution, using the Knuth-Yao algorithm.
 */
long discrete_gaussian::gen_number() {
    // Generate a bit for the sign.
    long sign = (RandomBits_long(1) == 0) ? 1 : -1;

    // Generate a number in the range [0,m*sigma] using the Knuth-Yao algorithm.
    int d = 0;
    int hit = 0;
    int col = 0;
    int s = 0;
    while (hit == 0 && col < sizeof(double) * 8) {
        long r = RandomBits_long(1);
        d = 2*d + r;

        for (int row = max - 1; row >= 0; row--) {
            d = d - get_bit(probs[row], col);
            if (d == -1) {
                s = row;
                hit = 1;
                break;
            }
        }

        col = col + 1;
    }

    return sign * s;
}

int main() {
    discrete_gaussian dg(2.5, 10);
    for (int i = 0; i < 10; i++) {
        cout << dg.gen_number() << endl;
    }
}

