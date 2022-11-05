#include <stdio.h>

int main() {
    int val;
    printf("Insert integer: ");
    scanf("%d", &val);
    printf("Included %d in range 33 - 88?\n %s\n", val, 33 < val && val < 88? "YES" : "NO");

    printf("Insert new integer: ");
    int new_val;
    scanf("%d", &new_val);
    int ans2 = (new_val >> 19) & 1;
    printf("%d", ans2);
    return 0;
}
