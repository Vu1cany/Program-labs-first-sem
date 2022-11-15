#include <stdio.h>
#include <string.h>

void delSpaces(char *string, char *stringRes) {
    if (*string == '\0') return;     //выход из рекурсси

    int i = strcspn(string, " ");   //Находим длинну строку до первого пробела
    strncat(stringRes, string, i);
    char lastEl = stringRes[strlen(stringRes) - 1];
    char *newString = &string[i];   //Указатель на строку начинающуюся с первого пробела

    if (strchr("({[<", lastEl)) {
        while (newString[0] == ' ') newString += 1;  //Записываем новое начало строки (убираем пробелы)
    }

    if (newString[0] == ' ' && newString[1] == ' ' && newString[2] == ' ') {
        while (newString[2] == ' ') newString = newString + 1;

        if (lastEl == '.') {    //Удаляет пробелы после последней точки
            if (newString[0] == ' ' && newString[1] == ' ' && newString[2] == ' ' && newString[3] == '\0') return;
            else if (newString[0] == ' ' && newString[1] == ' ' && newString[2] == '\0') return;
            else if (newString[0] == ' ' && newString[1] == '\0') return;
        }

        strncat(stringRes, newString, 2);
        newString += 2;
    } else {
        strncat(stringRes, newString, 1);
        newString += 1;
    }

    delSpaces(newString, stringRes);    //Снова вызов функции для правой части строки с пробелами
}

int main() {
    char string[100] = "", stringRes[100] = "";
    printf("Insert string:");
    gets(string);
    delSpaces(string, stringRes);
    printf("Result string: \n%s", stringRes);
    return 0;
}
