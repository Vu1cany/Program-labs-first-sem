#include <stdio.h>

int main() {
    int time, subFee, costMinute = 0;
    printf("Enter the duration of the conversations:");
    scanf("%d", &time);
    printf("Enter the amount of the subscription fee:");
    scanf("%d", &subFee);
    short limit = 499;
    while (costMinute <= subFee/limit) {
        printf("Enter the cost of a minute over the limit:");
        scanf("%d", &costMinute);
    }

    int fullCost = subFee;

    if (time > limit) {
        fullCost += (time - limit) * costMinute;
    }



    printf("Monthly maintenance cost: %d", fullCost);

    return 0;
}
