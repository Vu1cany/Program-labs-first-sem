#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <string.h>

int main(void) {
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    int var;
    char str[20];


    printf("������� ���� ������: ");
    gets(&str);
    printf("������� ����� �����: ");
    scanf("%d", &var);

    printf("�� ����� �����: ");
    printf("%d\n", var);
    printf("�� ����� ������: ");
    printf("%s\n", str);

    getch();
    return 0;
}