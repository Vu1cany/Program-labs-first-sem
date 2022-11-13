#include <stdio.h>
#include "Rings.h"

int main() {
    Ring myRing;

    printf("Insert minimal radius:");
    scanf("%lf", &myRing.radMin);
    printf("Insert maximum radius:");
    scanf("%lf", &myRing.radMax);

    Perimeter(&myRing);
    printf("%.2lf\n", myRing.P);
    Square(&myRing);
    printf("%.2lf", myRing.S);
    return 0;
}
