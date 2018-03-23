#include <iostream>

#include "discrete_gaussian.hpp"
#include "lwe_oracle.hpp"

using namespace std;

int main() {
    lwe_oracle l(5, ZZ(10), discrete_gaussian(RR(5),10));
    cout << l.get_s() << endl;
    for (int i = 0; i < 10; i++) {
        cout << l.query() << endl;
    }
}
