#ifndef LWE_ORACLE_HPP
#define LWE_ORACLE_HPP

#include <NTL/ZZ.h>
#include <NTL/vec_ZZ_p.h>

#include "discrete_gaussian.hpp"

using namespace NTL;

class lwe_oracle {
    private:
        long n; // The length of the vector.
        ZZ q; // The modulus. (We work over ZZ_q^n.)
        vec_ZZ_p s; // The secret vector.
        discrete_gaussian chi; // The distribution (over ZZ) of the error.
    public:
        lwe_oracle(long n, ZZ q, discrete_gaussian chi); // Choose secret uniformly over ZZ_q^n.
        lwe_oracle(long n, ZZ q, vec_ZZ_p s, discrete_gaussian chi); // Use provided secret.

        vec_ZZ_p query();

        // Getters.
        discrete_gaussian get_chi();
        long get_n();
        ZZ get_q();
        vec_ZZ_p get_s();
};

#endif

