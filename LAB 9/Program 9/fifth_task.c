/*
#include <stdio.h>

int main() {
    int monthsNum;
    double percent, capital;
    printf("Insert the number of months:");
    scanf("%d", &monthsNum);
    printf("Enter the number of percentages:");
    scanf("%lf", &percent);
    printf("Enter the initial amount of money:");
    scanf("%lf", &capital);
    double monthPercent = 1 + ((percent / 12) / 100);
    for (int i = 1; i <= monthsNum; ++i) {
        capital*= monthPercent;
        printf("Capital = %.2lf after %d months\n", capital, i);
    }

    return 0;
}
*/
