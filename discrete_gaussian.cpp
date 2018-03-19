#include <cmath>
#include <iostream>

#include <NTL/RR.h>
#include <NTL/ZZ.h>

#include "discrete_gaussian.hpp"

using namespace std;
using namespace NTL;

#define PREC 128 // Floating point precision.

/*
 * Initializes a discrete Gaussian generator with the provided standard deviation sigma and
 * number of standard deviations m.
 */
discrete_gaussian::discrete_gaussian(RR sigma, long m) : sigma(sigma), m(m) {
    // Initialize stuff.
    RR::SetPrecision(PREC);
    S = sigma * sqrt(2 * M_PI);
    max = conv<long>(ceil(m * sigma));
    bin_probs = (uint8_t **) malloc(8 * sizeof(uint8_t *));

    // Compute the probabilities for each x in [0,ceil(m*sigma)].
    probs = (RR *) calloc(max + 1, sizeof(RR));
    probs[0] = prob(0) / 2; // Use half the probability of 0, to account for +0 and -0.
    bin_probs[0] = (uint8_t *) malloc(RR::precision() / 8);
    to_bin(probs[0], bin_probs[0], RR::precision() / 8);
    for (long x = 1; x <= max; x++) {
        probs[x] = prob(x);
        bin_probs[x] = (uint8_t *) malloc(RR::precision() / 8);
        to_bin(probs[x], bin_probs[x], RR::precision() / 8);
    }
}

/*
 * Computes the probability of x according to this distribution. This is given exactly
 * by (1/S)*e^(-x^2 / 2*sigma^2), where S is a normalization factor, and we use the 
 * approximation S = sigma * sqrt(2 pi).
 */
RR discrete_gaussian::prob(long x) {
    RR x_real = conv<RR>(x);
    return (1 / S) * exp(-(x_real * x_real) / (2 * sigma * sigma));
}

/*
 * Computes PP(a <= x <= b) according to this distribution by summing up P(x = i) for i
 * in [a,b].
 */
RR discrete_gaussian::prob(long a, long b) {
    RR result(0);
    for (long x = a; x <= b; x++) {
        result += prob(x);
    }
    return result;
}

/*
 * Converts the provided RR into binary form, storing the binary representation in
 * the array arr with length len. The array is viewed as a binary string with length 8*len.
 */
void to_bin(RR d, uint8_t *arr, size_t len) {
    for (unsigned i = 0; i < len; i++) {
        arr[i] = 0;
    }

    for (unsigned i = 0; i < 8*len; i++) {
        d = d * 2;
        if (d >= 1) {
            d = d - 1;
            arr[i / 8] = arr[i / 8] ^ (1 << (7 - (i % 8)));
        }
    }
}

/*
 * Return the i-th bit (where 0 is the LEFTMOST bit) of the bit string stored in arr.
 * Assumes i is a valid index.
 */
int get_bit(uint8_t *arr, int i) {
    return ((arr[i / 8] >> (7 - i % 8)) & 1);
}

/*
 * Displays the internal binary probability table in a user-friendly format.
 */
void discrete_gaussian::display_bin_probs() {
    for (int i = 0; i <= max; i++) {
        cout << "0.";
        for (int j = 0; j <= RR::precision() / 8 * 8; j++) {
            cout << (get_bit(bin_probs[i],j) == 1 ? "1" : "0");
        }
        cout << endl;
    }
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
    unsigned col = 0;
    int s = 0;
    while (hit == 0) {
        long r = RandomBits_long(1);
        d = 2*d + (1 ^ r);

        for (int row = max; row >= 0; row--) {
            d = d - get_bit(bin_probs[row], col);
            if (d == -1) {
                s = row;
                hit = 1;
                break;
            }
        }

        col = col + 1;
        if (col > RR::precision() / 8 * 8) { // If we hit the end, try again.
            d = 0;
            col = 0;
        }
    }

    return sign * s;
}

