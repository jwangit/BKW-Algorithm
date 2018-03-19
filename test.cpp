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
    size_t len = 8;
    uint8_t arr[len];
    double d = 0.000000000000127;
    to_bin(d, arr, len);
    printf("0.");
    for (int i = 0; i < 8*len; i++) {
        int b = ((arr[i / 8] >> (7 - i % 8)) & 1);
        if (b == 0)
            printf("0");
        else if (b == 1)
            printf("1");
        else
            printf("E");
    }
    printf("\n");
    return 0;
    /*
    for (double d = 1.0; d >= 0; d -= 0.1) {
        for (int i = 0; i < sizeof(d) * 8; i++) {
            printf("%d", get_bit(d, i));
            if (i % 8 == 7)
                printf(" ");
        }
        printf("\n");
    }
    */
}

