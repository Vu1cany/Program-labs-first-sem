#include <stdio.h>
#include <math.h>

enum paints{
    Gouache,
    Watercolor,
    Oil,
    Water_based_emulsion
};

typedef struct {
    double X;
    double Y;
} Coords;

typedef struct {
    Coords A;
    Coords B;
    Coords C;
    double AB;
    double AC;
    double BC;
    double Perimeter;
} Triangle;

double Side(Coords A, Coords B) {
    double AB = sqrt(pow((A.X - B.X), 2) + pow((A.Y - B.Y), 2));
    return AB;
}

double P_triangle(double AB, double AC, double BC) {
    double P = AB + AC + BC;
    return P;
}

union CardReader{
    int input;
    struct {
        unsigned TurnedOn: 1;
        unsigned CD_card_activity: 1;
        unsigned CF_card_activity: 1;
        unsigned MS_card_activity: 1;
    };
};

int main() {
    enum paints PaintUsed = Oil;
    printf("Paint used: %d\n", PaintUsed);

    Triangle My_triangle;
    My_triangle.A.X = 0;
    My_triangle.A.Y = 0;
    My_triangle.B.X = 2;
    My_triangle.B.Y = 3;
    My_triangle.C.X = 4;
    My_triangle.C.Y = 0;
    My_triangle.AB = Side(My_triangle.A, My_triangle.B);
    My_triangle.AC = Side(My_triangle.A, My_triangle.C);
    My_triangle.BC = Side(My_triangle.B, My_triangle.C);
    My_triangle.Perimeter = P_triangle(My_triangle.AB, My_triangle.AC, My_triangle.BC);
    printf("Perimeter of the triangle = %.2f\n", My_triangle.Perimeter);

    union CardReader CR;
    printf("Enter a number: ");
    scanf("%X", &CR.input);
    printf("Turned on: ");
    printf(CR.TurnedOn? "YES\n":"NO\n");
    printf("Active CD: ");
    printf(CR.CD_card_activity? "YES\n":"NO\n");
    printf("Active CF: ");
    printf(CR.CF_card_activity? "YES\n":"NO\n");
    printf("Active MS: ");
    printf(CR.MS_card_activity? "YES\n":"NO\n");


    return 0;
}