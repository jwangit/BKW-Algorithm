#include <iostream>

#include <NTL/RR.h>
#include <NTL/ZZ.h>

using namespace std;
using namespace NTL;

/*
 * Return the i-th bit (where 0 is the LEFTMOST bit) of the floating point representation of d.
 */
int get_bit(double d, int i) {
    uint8_t *tmp = (uint8_t *) malloc(sizeof(double));
    memcpy(tmp, &d, sizeof(double));
    int result = ((tmp[i / 8] >> (i % 8)) & 1);
    free(tmp);
    tmp = NULL;
    return result;
}

int main() {
    for (double d = 1.0; d >= 0; d -= 0.1) {
        for (int i = 0; i < sizeof(d) * 8; i++) {
            printf("%d", get_bit(d, i));
            if (i % 8 == 7)
                printf(" ");
        }
        printf("\n");
    }
}

