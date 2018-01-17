#include <cmath>
#include <iostream>

#include <NTL/ZZ.h>

#include "discrete_gaussian.hpp"

using namespace std;
using namespace NTL;

/*
 * Initializes a discrete Gaussian generator with the provided standard deviation sigma,
 * number of standard deviations m, and block size t.
 */
discrete_gaussian::discrete_gaussian(double sigma, ZZ m) : sigma(sigma), m(conv<double>(m)), t(t) {
    // Initialize constants.
    S = sigma * sqrt(2 * M_PI);

    // Compute the blocks.
    double blockweight = 0.5 / t;
    blocks = new ky_block[t];
    blocks[0].a = 0;
    blocks[0].b = 0;
    blocks[0].total_prob = prob(ZZ(0)) / 2; // Use half the probability of 0, to account for +0 and -0.
    size_t curr_block = 0;
    for (ZZ x = ZZ(1); x <= conv<ZZ>(ceil(this->m * sigma)) && curr_block < t - 1; x++) {
        if (x <= ZZ(50))
            cout << "Computing for x = " << x << " and curr_block = " << curr_block << endl;
        double p_x = prob(x);
        double with = abs(blocks[curr_block].total_prob + p_x - blockweight);
        double without = abs(blocks[curr_block].total_prob - blockweight);
        cout << "with = " << with << ", without = " << without << endl;
        if (with <= without) {
        //if (blocks[curr_block].total_prob + p_x <= blockweight) {
            blocks[curr_block].b = x;
            blocks[curr_block].total_prob += p_x;
        } else {
            curr_block++;
            blocks[curr_block].a = x;
            blocks[curr_block].b = x;
            blocks[curr_block].total_prob = p_x;
        }
    }
    // Update t to match the actual number of blocks used.
    this->t = curr_block + 1;

    // Fill out the final block, just in case something terminated early.
    blocks[this->t - 1].b = conv<ZZ>(ceil(this->m * sigma));
    blocks[this->t - 1].total_prob = prob(blocks[this->t - 1].a, blocks[this->t - 1].b);
}

/*
 * Computes the probability of x according to this distribution. This is given exactly
 * by (1/S)*e^(-x^2 / 2*sigma^2), where S is a normalization factor, and we use the 
 * approximation S = sigma * sqrt(2 pi).
 */
double discrete_gaussian::prob(ZZ x) {
    double x_real = conv<double>(x);
    return (1 / S) * exp(-(x_real * x_real) / (2 * sigma * sigma));
}

/*
 * Computes PP(a <= x <= b) according to this distribution by summing up P(x = i) for i
 * in [a,b].
 */
double discrete_gaussian::prob(ZZ a, ZZ b) {
    double result = 0;
    for (ZZ x = a; x <= b; x++) {
        result += prob(x);
    }
    return result;
}

/*
 * Return the i-th bit (where 0 is the LEFTMOST bit) of the floating point representation of d.
 * Does not take endianness into account.
 */
int get_bit(double d, int i) {
    uint8_t *tmp = (uint8_t *) malloc(sizeof(double));
    memcpy(tmp, &d, sizeof(double));
    int result = ((tmp[i / 8] >> (i % 8)) & 1);
    free(tmp);
    tmp = NULL;
    return result;
}

/*
 * Generates a random number according to this distribution, using the Knuth-Yao algorithm
 * with blocks.
 */
ZZ discrete_gaussian::gen_number() {
    // Generate a bit for the sign.
    ZZ sign = (RandomBits_long(1) == 0) ? ZZ(1) : ZZ(-1);

    // Generate a number in the range [0,m*sigma] using the Knuth-Yao algorithm with
    // blocks.
    
    // First use the Knuth-Yao algorithm on the block list to find a block.
    int d = 0;
    int hit = 0;
    int col = 0;
    while (hit == 0) {
        long r = RandomBits_long(1);
        d = 2*d + r;

        for (int row = t - 1; row >= 0; row--) {
            d = d - get_bit(
        }
    }

    return ZZ(0);
}

int main() {
    discrete_gaussian dg(5, ZZ(10), 10);
    double total = 0;
    for (ZZ i = ZZ(0); i < 50; i++) {
        double p_i = dg.prob(i);
        cout << "P(x = " << i << ") = " << p_i << endl;
        total += p_i;
    }
    cout << "Total: " << total - (dg.prob(ZZ(0)) / 2) << endl << endl;

    cout << "Blocks of 50-40-10 distribution:" << endl;
    dg = discrete_gaussian(50, ZZ(40), 10);
    ky_block *blocks = dg.get_blocks();
    for (size_t i = 0; i < dg.get_t(); i++) {
        cout << blocks[i] << endl;
    }

    cout << dg.prob(ZZ(0),ZZ(12)) << endl;
    cout << dg.prob(ZZ(13),ZZ(25)) << endl;
    cout << dg.prob(ZZ(26),ZZ(39)) << endl;
    cout << dg.prob(ZZ(40),ZZ(54)) << endl;
    cout << dg.prob(ZZ(55),ZZ(70)) << endl;
    cout << dg.prob(ZZ(71),ZZ(88)) << endl;
    cout << dg.prob(ZZ(89),ZZ(109)) << endl;
    cout << dg.prob(ZZ(110),ZZ(136)) << endl;
    cout << dg.prob(ZZ(137),ZZ(180)) << endl;
    cout << dg.prob(ZZ(181),ZZ(2000)) << endl << endl;

    dg = discrete_gaussian(10, ZZ(20), 10);
    cout << dg.prob(ZZ(0)) << endl;
    cout << dg.prob(ZZ(1)) << endl;
    cout << dg.prob(ZZ(10)) << endl;
    cout << dg.prob(ZZ(20)) << endl;
    cout << dg.prob(ZZ(30)) << endl;
    cout << dg.prob(ZZ(40)) << endl;
    cout << dg.prob(ZZ(50)) << endl;
}

