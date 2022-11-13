#include <stdio.h>

int main() {
    int time, subFee, costMinute;
    printf("Enter the duration of the conversations:");
    scanf("%d", &time);
    printf("Enter the amount of the subscription fee:");
    scanf("%d", &subFee);
    printf("Enter the cost of a minute over the limit:");
    scanf("%d", &costMinute);
    short limit = 499;
    int fullCost = subFee;

    if (time > limit) {
        fullCost += (time - limit) * costMinute;
    }

    printf("Monthly maintenance cost: %d", fullCost);

    return 0;
}
