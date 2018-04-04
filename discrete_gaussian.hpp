#ifndef DISCRETE_GAUSSIAN_HPP
#define DISCRETE_GAUSSIAN_HPP

#include <fstream>
#include <iostream>

#include <NTL/ZZ.h>
#include <NTL/RR.h>

using namespace std;
using namespace NTL;

/*
 * Represents a block, containing a closed interval [a,b] and the total probability P([a,b]).
 * /
struct ky_block {
    ZZ a, b;
    RR total_prob;
};

ostream& operator<<(ostream& os, const ky_block& blk) {
    return os << "[" << blk.a << "," << blk.b << "]: " << blk.total_prob;
}
*/

/*
 * Represents a generator for true random bits.
 */
class random_bit_generator {
    private:
        basic_ifstream<uint8_t> randsource; // Source file of random bits.
        uint8_t curr; // Current random byte we're using.
        uint8_t pos; // Current position in that random byte.

    public:
        /*
         * Construct a new random bit generator using the provided 
         * source of randomness (e.g., /dev/random).
         */
        random_bit_generator(string randomfilename) {
            randsource.open(randomfilename, ios::in | ios::binary);
            randsource.get(curr);
            pos = 0;
        }

        ~random_bit_generator() {
            randsource.close();
        }

        /*
         * Get the next random bit, as an integer valued 0 or 1. 
         */
        int next_bit() {
            int result = curr & 1;
            curr = curr >> 1;
            pos++;
            
            // If we run out of bits from the current random byte, 
            // get a new byte and start again.
            if (pos >= 8) {
                randsource.get(curr);
                pos = 0;
            }

            return result;
        }
};

void to_bin(RR d, uint8_t *arr, size_t len);

/*
 * Represents a Knuth-Yao discrete Gaussian random number generator, centered around 0. 
 * Generates integers in the range [-ceil(m * sigma), ceil(m * sigma)].
 */
class discrete_gaussian {
    private:
        RR sigma; // The standard deviation of the distribution.
        long m; // The number of standard deviations we want to consider. Typically
                // in the range [20,40].
        long max; // The maximum value supported by this distribution. Given by
                  // ceil(m*sigma).
        RR *probs; // The list of probabilities for each x in [0,max].
        uint8_t **bin_probs; // The list of probabilities in binary format.
        RR S; // Normalization factor.
    public:
        discrete_gaussian(RR sigma, long m);

        RR prob(long x);
        RR prob(long a, long b);

        void display_bin_probs();

        ZZ gen_number();

        RR get_sigma() { return sigma; }
        long get_m() { return m; }
        long get_max() { return max; }
};

#endif
