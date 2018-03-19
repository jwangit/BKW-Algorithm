#include <cmath>
#include <iostream>

#include <NTL/RR.h>
#include <NTL/ZZ.h>

#include "discrete_gaussian.hpp"

using namespace std;
using namespace NTL;

int main() {
    int arr[2*25 + 2] = {0};
    RR total(1000000);
    discrete_gaussian dg(RR(2.5), 10);
    for (int i = 0; i < total; i++) {
        int n = dg.gen_number();
        arr[n + 25] = arr[n + 25] + 1;
    }
    dg.display_bin_probs();

    cout << endl;
    cout << "================Observed================" << endl;
    for (int i = -25; i <= 25; i++) {
        cout << i << ": " << (arr[i + 25] / total) << endl;
    }
    cout << endl;
    cout << "================Expected================" << endl;
    for (int i = -25; i <= 25; i++) {
        cout << i << ": " << dg.prob(i) << endl;
    }
    cout << endl;
    cout << "===============Difference===============" << endl;
    for (int i = -25; i <= 25; i++) {
        cout << i << ": " << (arr[i + 25] / total) - dg.prob(i) << endl;
    }
}

