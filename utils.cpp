#include <unordered_map>

#include <NTL/ZZ.h>
#include <NTL/ZZ_p.h>
#include <NTL/vec_ZZ.h>
#include <NTL/vec_ZZ_p.h>

#include "utils.hpp"

using namespace std;
using namespace NTL;

/*
 * Tests whether the slice v_(a,b) is entirely zero or not. The indicies a and b start from
 * 0. For convenience, this function will return true for a >= b.
 */
bool is_all_zero(vec_ZZ_p v, int a, int b) {
    for (int i = a; i < b; i++)
        if (v[i] != 0)
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
 * Creates and returns the zero vector in Z_p^n.
 */
vec_ZZ_p zero_vector(long n) {
    vec_ZZ_p z;
    z.SetLength(n);
    for (int i = 1; i <= n; i++) {
        z(i) = 0;
    }
    return z;
}

/*
 * Returns the "next" vector after v. We increment the last component by one, and if the 
 * resulting value is 0, we increment the one before that, and so on.
 */
vec_ZZ_p next_vector(vec_ZZ_p v) {
    vec_ZZ_p result = v;
    for (int i = v.length(); i >= 1; i--) {
        result(i) = result(i) + 1;
        if (result(i) != 0)
            return result;
    }
    return result;
}
