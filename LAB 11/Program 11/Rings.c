#include <math.h>
#include "Rings.h"

void Perimeter(Ring *ring1) {
    ring1->P = 2 * M_PI * (ring1->radMax + ring1->radMin);
}

void Square(Ring *ring1) {
    ring1->S = M_PI * (pow(ring1->radMax, 2) - pow(ring1->radMin, 2));
}
