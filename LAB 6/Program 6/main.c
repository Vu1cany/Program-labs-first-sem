#include <stdio.h>
#include <stdlib.h>

int main() {
    const int n = 4;
    int *arr[n];

    for (int i = 0; i < n; ++i) {
        scanf("%d", &arr[i]);
    }
    for (int i = 0; i < n; ++i) {
        printf("%d ", *(arr+i));
    }
    printf("\n");

    int *arr2 = (int*) calloc(n, sizeof(int));

    for (int i = 0; i < n; ++i) {
        scanf("%d", &arr2[i]);
    }

    for (int i = 0; i < n; ++i) {
        printf("%d ", arr2[i]);
    }
    free(arr2);
    return 0;
}
