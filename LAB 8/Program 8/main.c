#include <stdio.h>
#include <string.h>

int main() {
    char string1[50], string2[50];
    printf("String Comparison \n");
    printf("Insert string_1:");
    gets(string1);
    printf("Insert string_2:");
    gets(string2);
    printf("string_1 and string_2 are ");
    printf(strcmp(string1, string2)? "not similar\n":"similar\n");

    int n;
    printf("Write the number of characters to copy:");
    scanf("%d", &n);
    strncpy(string1, string2, n);
    printf("%s\n",string1);
    printf("%s\n", string2);

    printf("string_1 size = %llu\n", strlen(string1));
    printf("string_2 size = %llu\n", strlen(string2));

    char foundedChar = strpbrk(string1, string2)[0];
    printf("In string_1 was found \"%c\" from string_2\n", foundedChar);
    char new_string1[50];
    char new_string2[5] = ", _.";
    char *PartString;
    printf("Insert new string:");
    getchar();
    gets(new_string1);
    printf("Parts of the string:\n");
    PartString = strtok(new_string1, new_string2);
    while (PartString != NULL) {
        printf("%s\n", PartString);
        PartString = strtok(NULL, new_string2);
    }

    return 0;
}

