#include <iostream>

#include <NTL/RR.h>
#include <NTL/ZZ.h>

#include "discrete_gaussian.hpp"

using namespace std;
using namespace NTL;

/*
 * Return the i-th bit (where 0 is the LEFTMOST bit) of the floating point representation of d.
 */
/*
int get_bit(double d, int i) {
    uint8_t *tmp = (uint8_t *) malloc(sizeof(double));
    memcpy(tmp, &d, sizeof(double));
    int result = ((tmp[i / 8] >> (i % 8)) & 1);
    free(tmp);
    tmp = NULL;
    return result;
}
*/

int main() {
    random_bit_generator rbg;
    for (int i = 0; i < 100; i++) {
        printf("%d", rbg.next_bit());
    }
    printf("\n");
    return 0;
}

