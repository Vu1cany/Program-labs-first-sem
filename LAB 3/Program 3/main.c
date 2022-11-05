#include <stdio.h>
#include <math.h>

int main() {
    printf("Insert your value in 10cc: ");
    int val;
    scanf("%d", &val);
    printf("Your value in 16cc: %X\n", val);
    printf("Your value in 16cc << 4: %X\n", val << 4);
    printf("Your value in 16cc^ %X\n", val);
    printf("Your value in 16cc with ~: %X\n", ~val);
    int newVal;
    printf("Insert your second value in 16cc: ");
    //scanf("%X", &newVal);
    printf("Your second value & your first value in 16cc: %X\n", newVal & val);

    printf("Your value in 2cc: ");
    for (int i = 31; i >= 0; --i) {
        if (val < pow(2,i)) { continue; }
        printf("%d", (val >> i) & 1);
    }
}