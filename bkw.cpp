#include <iostream>
#include <unordered_map>
#include <unordered_set>

#include <NTL/ZZ.h>
#include <NTL/ZZ_p.h>
#include <NTL/vec_ZZ.h>
#include <NTL/vec_ZZ_p.h>

#include "discrete_gaussian.hpp"
#include "lwe_oracle.hpp"
#include "utils.hpp"

using namespace std;
using namespace NTL;

/*
 * Class representing the series of "B oracles" we will be using for the sample reduction
 * stage of the algorithm.
 */
class B_oracles {
    private:
        vecmap *Tl; // Array of Tl tables, one for each l (0 < l < a).
        lwe_oracle &oracle;
        long a;
        long b;
        long d;
    public:
        /*
         * Construct the series of B oracles we will use. These will keep track of the
         * provided parameters and maintain the Tl tables needed for all instances of
         * the oracle.
         */
        B_oracles(lwe_oracle &oracle, long b, long d) : oracle(oracle), b(b), d(d) {
            this->a = ceil(oracle.get_n() / b);
            this->Tl = new vecmap[a + 1];
            for (int i = 0; i <= a; i++) {
                Tl[i] = vecmap();
            }
        }

        /*
         * Query the oracle indexed by l (0 <= l <= a) and return the result.
         */
        vec_ZZ_p query(long l) {
            // If l = 0, this is just the LWE oracle.
            if (l <= 0) {
                return oracle.query();
            }

            // If l = a, this is the special case where we want to eliminate all but the
            // last d components. 
            if (l == a) {
                // Query the (a - 1) oracle to obtain ac = (a,c).
                vec_ZZ_p ac = this->query(a - 1);

                // If the b*(a - 1)-th through (n - d)-th components of a are all zeros,
                // return (a,c). Note that the algorithm is guaranteed to terminate here
                // when we choose d = b.
                long n = oracle.get_n();
                vec_ZZ_p ac_slice = slice(ac, b*(a - 1), n - d);
                if (is_all_zero(ac, b*(a - 1), n - d))
                    return ac;

                // Repeatedly query the (a - 1) oracle until we either hit an all-zero 
                // slice or we find two vectors whose b*(a - 1)-th through (n - d)-th
                // components all sum to zero.
                while (Tl[a][ac_slice].length() == 0 && Tl[a][-ac_slice].length() == 0) {
                    Tl[a][ac_slice] = ac;
                    ac = this->query(a - 1);
                    ac_slice = slice(ac, b*(a - 1), n - d);
                    if (is_all_zero(ac, b*(a - 1), n - d))
                        return ac;
                }

                // Now that we've found two vectors whose b*(a - 1)-th through (n - d)-th
                // components differ by a multiple of -1, return their difference or sum
                // (whichever causes these components to become zero).
                if (Tl[a][ac_slice].length() != 0) {
                    vec_ZZ_p ac_prime = Tl[a][ac_slice];
                    return ac - ac_prime;
                } else {
                    vec_ZZ_p ac_prime = Tl[a][-ac_slice];
                    return ac + ac_prime;
                }
            }

            // For 0 < l < a, query the previous oracle to obtain ac = (a,c).
            vec_ZZ_p ac = this->query(l - 1);

            // If the b*(l - 1)-th component through the b*l-th component of a are all
            // zeros, return (a,c).
            if (is_all_zero(ac, b*(l - 1), b*l))
                return ac;

            // Repeatedly query the previous oracle until we either hit an all-zero
            // slice or we find two vectors whose b*(l - 1)-th through (b*l - 1)-th components
            // are either identical or all sum to zero.
            vec_ZZ_p ac_slice = slice(ac, b*(l - 1), b*l);
            while (Tl[l][ac_slice].length() == 0 && Tl[l][-ac_slice].length() == 0) {
                Tl[l][ac_slice] = ac;
                ac = this->query(l - 1);
                ac_slice = slice(ac, b*(l - 1), b*l);
                if (is_all_zero(ac, b*(l - 1), b*l))
                    return ac;
            }

            // Now that we've found two vectors whose b*(l - 1)-th through b*l-th components differ by 
            // a multiple of -1, return their difference or sum (whichever causes these components to 
            // become zero).
            if (Tl[l][ac_slice].length() != 0) {
                vec_ZZ_p ac_prime = Tl[l][ac_slice];
                return ac - ac_prime;
            } else {
                vec_ZZ_p ac_prime = Tl[l][-ac_slice];
                return ac + ac_prime;
            }
        }
};

/*
 * Performs the Search version of the BKW algorithm to recover the secret vector 
 * s from the provided oracle. When it completes, it returns s.
 *
 * The algorithm requires the following parameters:
 *     b: the "window width."
 *     d: the length of the last block we will eliminate (0 <= d < b).
 *     m: the number of samples from the a-th B oracle we will use to conduct
 *        hypothesis testing.
 */
vec_ZZ_p bkw(lwe_oracle &oracle, long b, long d, long m) {
    // Initialize some helpful constants.
    const long n = oracle.get_n();
    const long q = conv<long>(oracle.get_q());
    const long a = ceil(n / b);
    discrete_gaussian chi = oracle.get_chi();
    
    /****************************
     * Step 1: Sample Reduction *
     ****************************/

    // Set up the series of B oracles.
    B_oracles bs(oracle, b, d);

    // Query the a-th B oracle m times and store them in an array F.
    vec_ZZ_p *F = new vec_ZZ_p[m];
    for (long i = 0; i < m; i++) {
        F[i] = bs.query(a);
        cerr << F[i] << endl;
    }

    /*******************************
     * Step 2: Hypothesis Testing. *
     *******************************/

    // Initialize the distribution of the errors of the B oracles, which will be used in
    // the computation of the scores. The distribution is the discrete Gaussian with mean
    // 0 and standard deviation sqrt(2^a) * sigma, where sigma is the standard deviation of the
    // original error distribution.
    discrete_gaussian chi_a(sqrt(power_long(2,a)) * chi.get_sigma(), chi.get_m());

    // Initialize the array S, which will contain the scores of each candidate.
    unordered_map<vec_ZZ_p,RR> S;
 
    // Go through each of the possible candidate vectors v in Z_q^d.
    vec_ZZ_p v = zero_vector(d);
    for (long tmp = 0; tmp < power_long(q, d); tmp++) {
        S[v] = RR(0); // Set the initial score for this v to 0.

        // Go through each vector F[i] = (a_i,c_i).
        for (long i = 0; i < m; i++) {
            // Let a_i_prime be the last d components of a_i. Compute j = <a_i_prime,v> - c_i.
            vec_ZZ_p a_i_prime = slice(F[i], n - d, n);
            ZZ_p j = a_i_prime * v - F[i](n + 1);

            // Compute the weight W_j = log_2[(q^d - 1)p_j/(q^(d - 1) - p_j)], where
            // p_j = Pr[e ~ chi_a : e = j].
            RR p_j = chi_a.prob(conv<long>(j));
            RR p_j_tilde = (power_long(q,d - 1) - p_j)/(power_long(q,d) - 1);
            cout << "j = " << j << "; p_j = " << p_j << "; p_j_tilde = " << p_j_tilde << endl;
            RR W_j = log(p_j/p_j_tilde)/log(RR(2));

            // Add W_j/m to the score.
            S[v] = S[v] + W_j/m;
            
            v = next_vector(v);
        }
    }

    // Find the maximum score.
    vec_ZZ_p max_vec = zero_vector(d);
    RR max_val = S[max_vec];
    for (pair<vec_ZZ_p,RR> element : S) {
        cout << element.first << ": " << element.second << endl;
        if (element.second >= max_val) { 
            max_vec = element.first;
            max_val = element.second;
        }
    }

    cout << "Predicted value: " << max_vec << endl;
    cout << "Actual value: " << oracle.get_s() << endl;

    delete [] F;
    return random_vec_ZZ_p(3);
}

int main() {
    ZZ q(19);
    ZZ_pPush push(q);
    lwe_oracle l(8, q, discrete_gaussian(RR(2.5),10));
    vec_ZZ_p result = bkw(l, 2, 1, 500);
}

