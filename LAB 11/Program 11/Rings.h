#ifndef RINGS_H
#define RINGS_H

typedef struct {
    double x;
    double y;
} Coords;

typedef struct {
    Coords center;
    double radMin;
    double radMax;
    double P;
    double S;
}Ring;

extern void Perimeter(Ring *ring1);

extern void Square(Ring *ring1);

#endif
