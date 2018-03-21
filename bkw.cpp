#include <iostream>
#include <unordered_map>

#include <NTL/ZZ.h>
#include <NTL/ZZ_p.h>
#include <NTL/vec_ZZ.h>
#include <NTL/vec_ZZ_p.h>

#include "discrete_gaussian.hpp"
#include "lwe_oracle.hpp"

using namespace std;
using namespace NTL;

/*
 * Class that hashes a vec_ZZ_p, so we can use them in unordered_maps.
 */
template<> struct std::hash<vec_ZZ_p> {
    public:
        /*
         * The hash of a vector v of length n will be the number obtained when
         * we view v as a number in base p with n digits. As such, there will
         * be no collisions among vectors of the same length (which will generally
         * be our case).
         */
        size_t operator()(const vec_ZZ_p& v) const {
            size_t n = v.length();
            size_t p = conv<size_t>(ZZ_p::modulus());
            size_t hash = 0;
            for (size_t i = 1; i <= n; i++) {
                hash += conv<size_t>(v(i)) * power_long(p, n - i);
            }
            return hash;
        }
};

using vecmap = unordered_map<vec_ZZ_p, vec_ZZ_p>;

/*
 * Tests whether the subvector v_(a,b) is entirely zero or not. The indicies
 * a and b start from 1.
 */
bool is_all_zero(vec_ZZ_p v, int a, int b) {
    for (int i = a; i <= b; i++)
        if (v(i) != 0)
            return false;

    return true;
}

/*
 * Returns the a-th through (b - 1)-th components of the vector v as a vectors. The indexing
 * starts at 0.
 */
vec_ZZ_p slice(vec_ZZ_p v, int a, int b) {
    vec_ZZ_p res = random_vec_ZZ_p(b - a);
    for (int i = a; i <= b - 1; i++) {
        res[i - a] = v[i];
    }
    
    return res;
}

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

        // Tests whether or not the table Tl[i] has an entry for the provided vector v.
        bool Tl_empty(int i, vec_ZZ_p v) {
            return Tl[i].find(v) == Tl[i].end();
        }
    public:
        /*
         * Construct the series of B oracles we will use. These will keep track of the
         * provided parameters and maintain the Tl tables needed for all instances of
         * the oracle.
         */
        B_oracles(lwe_oracle &oracle, long b) : oracle(oracle), b(b) {
            this->a = ceil(oracle.get_n() / b);
            this->Tl = (vecmap *) malloc(a * sizeof(vecmap));
            for (int i = 0; i < a; i++) {
                Tl[i] = vecmap();
            }
        }

        // Query the oracle indexed by l (0 <= l < a) and return the result.
        vec_ZZ_p query(int l) {
            // If l = 0, this is just the LWE oracle.
            if (l <= 0) {
                return oracle.query();
            }

            // For 0 < l < a, query the previous oracle to obtain ac = (a,c).
            vec_ZZ_p ac = this->query(l - 1);

            // If the b*(l - 1)-th component through the b*l-th component of a are all
            // zeros, return (a,c).
            if (is_all_zero(ac, b*(l - 1), b*l))
                return ac;

            // Repeatedly query the previous oracle until we either hit an all-zero
            // subvector or we find two vectors whose b*(l - 1)-th through b*l-th components
            // all sum to zero.
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
 * The algorithm requires a parameter b, the "window width."
 */
vec_ZZ_p bkw(lwe_oracle &oracle, long b) {
    // Set up the series of B oracles.
    B_oracles bs(oracle, b);

    return bs.query(3);
}

static discrete_gaussian dg(RR(2.5),10);

int main() {
    ZZ_pPush push(ZZ(10));
    lwe_oracle l(8, ZZ(10), [](){ return dg.gen_number(); });
    cout << "Result: " << bkw(l,2) << endl;
}

