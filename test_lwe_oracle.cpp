#include <iostream>

#include "discrete_gaussian.hpp"
#include "lwe_oracle.hpp"

using namespace std;

static discrete_gaussian dg(RR(5), 10);

int main() {
    lwe_oracle l(5, ZZ(10), [](){return dg.gen_number();});
    cout << l.get_s() << endl;
    for (int i = 0; i < 10; i++) {
        cout << l.query() << endl;
    }
}
