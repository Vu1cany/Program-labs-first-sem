#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <math.h>

int main(void) {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    float z1,z2;
    int a;

    printf("Введите значение перменной \"a\": ");
    scanf("%d", &a);
    z1 = ((a+2)/sqrt(2*a) - a/(sqrt(2*a) + 2) + 2/(a - sqrt(2*a))) * ((sqrt(a) - sqrt(2))/(a + 2));
    z2 = 1/(sqrt(a)+sqrt(2));

    printf("Значение z1 = %f\n", z1);
    printf("Значение z2 = %f", z2);

    getch();
    return 0;
}