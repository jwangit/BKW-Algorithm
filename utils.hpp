#ifndef UTILS_HPP
#define UTILS_HPP

#include <unordered_map>

#include <NTL/ZZ.h>
#include <NTL/ZZ_p.h>
#include <NTL/vec_ZZ.h>
#include <NTL/vec_ZZ_p.h>

using namespace std;
using namespace NTL;

/*
 * struct that hashes a vec_ZZ_p, so we can use them in unordered_maps.
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

bool is_all_zero(vec_ZZ_p v, int a, int b);
vec_ZZ_p slice(vec_ZZ_p v, int a, int b);
vec_ZZ_p zero_vector(long n);
vec_ZZ_p next_vector(vec_ZZ_p v);

#endif
