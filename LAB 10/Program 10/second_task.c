#include <stdio.h>
#include <math.h>
#include <stdlib.h>

typedef struct {
    double x;
    double y;
} Coords;

double **distances(Coords *arr,  int n) {
    double **dist = malloc(sizeof(double *) * n);
    for (int i = 0; i < n; ++i) dist[i] = malloc(sizeof(double) * (n-1));
    for (int i = 0; i < n; ++i) {
        int count = 0;
        for (int j = 0; j < n; ++j) {
            if (j == i) continue;
            dist[i][count] = sqrt((arr[i].x - arr[j].x) * (arr[i].x - arr[j].x) + (arr[i].y - arr[j].y) * (arr[i].y - arr[j].y));
            count++;
        }
    }
    return dist;
}

int main() {
    int n;
    printf("Insert the number of points:");
    scanf("%d", &n);
    Coords arr[n];
    for (int i = 0; i < n; ++i) {
        printf("Insert X for the %d point:", i+1);
        scanf("%lf", &arr[i].x);
        printf("Insert Y for the %d point:", i+1);
        scanf("%lf", &arr[i].y);
    }
    double **distArr = distances(arr, n);

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n-1; ++j) {
            printf("%.2lf ", distArr[i][j]);
        }
        printf("\n");
    }


    return 0;
}
