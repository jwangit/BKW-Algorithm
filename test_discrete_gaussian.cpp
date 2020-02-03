#include <iostream>
#include <cmath>
#include <chrono>
#include <NTL/RR.h>
#include <NTL/ZZ.h>
#include "discrete_gaussian.hpp"
#include <cstdlib>
#include <math.h>
//#include "D:/Gaussian Sampling/BKZalgorithm/BKWAlgorithm/BKWAlgorithm/Samplers.h"

using namespace std;
using namespace NTL;

int main() {
	double speed = 0;
//	int Num = 64;
//    int arr[2*25 + 2] = {0};
//	int arr[2 * 64 + 2] = { 0 };
//    RR total(1000000);
	RR total(100000);
    discrete_gaussian dg(RR(256), 8);
//	discrete_gaussian dg(RR(256), 8);

	auto start = chrono::steady_clock::now();
	for (int i = 0; i < total;i++) {
		dg.gen_number();
	}
	auto end = chrono::steady_clock::now();
	speed = 100000.0 / chrono::duration_cast<chrono::milliseconds>(end - start).count() * 1000;
	cout << "Elapsed time in ms:" << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;
	cout.precision(15);
	cout << "speed:" << speed << " samples/s" << endl;


	//for (int i = 0; i < total; i++) {
    //    long n = conv<long>(dg.gen_number());
    //    arr[n + 15] = arr[n + 15] + 1;
    //}
    //dg.display_bin_probs();

    //cout << endl;
    //cout << "================Observed================" << endl;
    //for (int i = -Num; i <= Num; i++) {
    //    cout << i << ": " << (arr[i + Num] / total) << endl;
    //}
    //cout << endl;
    //cout << "================Expected================" << endl;
    //for (int i = -15; i <= 15; i++) {
    //    cout << i << ": " << dg.prob(i) << endl;
    //}
    //cout << endl;
    //cout << "===============Difference===============" << endl;
    //for (int i = -15; i <= 15; i++) {
    //    cout << i << ": " << (arr[i + 15] / total) - dg.prob(i) << endl;
    // }

	//Samplers KnuthYao(int tailcut, RR sigma, RR c);

}

