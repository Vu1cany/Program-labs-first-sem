#include <stdio.h>

void MatFill(int arr[2][2], int r) {
    for (int i = 0; i < r; ++i) {
        for (int j = 0; j < r; ++j) {
            scanf("%d", &arr[i][j]);
        }
    }
}

int main() {
    const int n = 6;
    int arr[n];

    for (int i = 0; i < n; ++i) {
        scanf("%d", &arr[i]);
    }
    for (int i = 0; i < n; ++i) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    const int r = 2;
    int mat1[r][r], mat2[r][r];

    printf("Fill the first matrix: \n");
    MatFill(mat1, r);
    printf("Fill the second matrix: \n");
    MatFill(mat2, r);

    int result[r][r];
    for (int i = 0; i < r; ++i) {
        for (int j = 0; j < r; ++j) {
            result[i][j] = mat1[i][0]*mat2[0][j] + mat1[i][1]*mat2[1][j];
        }
    }
    printf("Result: \n");
    for (int i = 0; i < r; ++i) {
        for (int j = 0; j < r; ++j) {
            printf("%d ", result[i][j]);
        }
        printf("\n");
    }
    return 0;
}
