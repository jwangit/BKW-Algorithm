#include <iostream>

#include "lwe_oracle.hpp"

using namespace std;
using namespace NTL;

/*
 * Create a new LWE oracle over ZZ_q^n, using a randomly generated secret vector and the provided
 * error distribution chi.
 */
lwe_oracle::lwe_oracle(long n, ZZ q, ZZ (*chi)()) : n(n), q(q), chi(chi) {
    // Generate a secret vector uniformly at random.
    ZZ_pPush push(q);
    s = random_vec_ZZ_p(n);
}

/*
 * Create a new LWE oracle over ZZ_q^n, using the provided secret vector s and error distribution chi.
 */
lwe_oracle::lwe_oracle(long n, ZZ q, vec_ZZ_p s, ZZ (*chi)()) : n(n), q(q), s(s), chi(chi) {}

/*
 * Query the LWE oracle. Returns a vector in (ZZ_q^n x ZZ_q).
 */
vec_ZZ_p lwe_oracle::query() {
    // Save the old modulus and set the modulus to q. Will be restored when the function exits.
    ZZ_pPush push(q);

    // Generate a uniform random vector a in ZZ_q^n and an error e in ZZ_p, sampled according to chi
    // and reduced mod q.
    vec_ZZ_p a = random_vec_ZZ_p(n);
    ZZ_p e = conv<ZZ_p>(chi());

    // Compute and return (a,c), where c = <a,s> + e.
    ZZ_p c = (a * s) + e;
    vec_ZZ_p result(a);
    result.SetLength(n + 1);
    result(n + 1) = c;
    return result;
}

/*
 * Get the vector length.
 */
long lwe_oracle::get_n() { 
    return n;
}

/*
 * Get the modulus.
 */
ZZ lwe_oracle::get_q() {
    return q;
}

/*
 * Get the secret vector. Obviously this isn't available for real-world examples, but
 * it's helpful for testing purposes and such.
 */
vec_ZZ_p lwe_oracle::get_s() {
    return s;
}

// Tester
int main() {
    lwe_oracle l(5, ZZ(10), [](){ return RandomLen_ZZ(64); });
    cout << l.get_s() << endl;
    for (int i = 0; i < 10; i++) {
        cout << l.query() << endl;
    }
}
