#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <string.h>

int main(void) {
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    int var;
    char str[20];


    printf("Введите вашу строку: ");
    gets(&str);
    printf("Введите целое число: ");
    scanf("%d", &var);

    printf("Вы ввели число: ");
    printf("%d\n", var);
    printf("Вы ввели строку: ");
    printf("%s\n", str);

    getch();
    return 0;
}