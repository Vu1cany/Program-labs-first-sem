#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(1) //выравнивание в 1 байт

typedef struct {
    unsigned char Type[2];   //Тип файла ("BM")
    unsigned int Size;   //Размер файла в байтах
    unsigned int Reserved1;    //зарезервировано Reserved1 и Reserved2 (нули)
    unsigned int OffBits;    //Смещение до массива массива из пикселей относительно начала файла
}BitMapFileHeader;

typedef struct{
    unsigned int SizeInfoHeader;   //Размер структуры
    unsigned int Width;  //Ширина изображения в пикселах.
    unsigned int Height;   //Высота изображения в пикселах.
    unsigned short Planes;   //Количество плоскостей
    unsigned short BitCount;   //Глубина цвета в битах на пиксель
    unsigned int Compression;   //Тип сжатия. Если компрессия не используется, то флаг имеет значенине BI_RGB.
    unsigned int SizeImage;  //Размер изображения в байтах. (Если сжатия нет, то может быть написан ноль)
    unsigned int XPixelPerMeter;  //Горизонтальное разрешение (в пикселях на метр). Важно в первую очередь для устройства, на которое будет выводиться битовый массив.
    unsigned int YPixelPerMeter;  //Вертикальное разрешение (в пикселях на метр). Важно в первую очередь для устройства, на которое будет выводиться битовый массив.
    unsigned int ColorsUsed;    //Количество используемых цветов кодовой таблицы.
    unsigned int ColorsImportant;  //Количество основных цветов. Если значение поля равно 0, то используются все цвета.
}BitMapInfoHeader;

typedef struct {
    BitMapFileHeader fileHeader;
    BitMapInfoHeader InfoHeader;
    unsigned int ColorTable[256];   //Палитра цветов
    char** arr;     //Для массива пикселей
}BMP;

#pragma pop

BMP* ReadBMPFile(char *fileName) {   //Функция для считывания BMP
    FILE* file = fopen(fileName, "rb");  //Открываем поток на чтение

    if (file == NULL) {     //если файл не удалось открыть
        printf("\nERROR: COULD\'T OPEN THE FILE NAMED \"%s\"\n\n", fileName);
        return NULL;
    }

    BMP* bmp = malloc(sizeof(BMP));

    fread(&bmp->fileHeader, sizeof(BitMapFileHeader), 1, file); //читаем структуру заголовка файла
    fread(&bmp->InfoHeader, sizeof(BitMapInfoHeader), 1, file); //заголовка информации
    fread(&bmp->ColorTable, sizeof(bmp->ColorTable), 1, file);  //Читаем байты политры цветов

    bmp->arr = malloc(bmp->InfoHeader.Height * sizeof(unsigned char*));
    for (int i = 0; i < bmp->InfoHeader.Height; ++i) {  //Выделяем память для массива пикселей
        bmp->arr[i] = malloc(bmp->InfoHeader.Width * sizeof(unsigned char) + (3*bmp->InfoHeader.Width % 4));
    }

    for (int i = 0; i < bmp->InfoHeader.Height; ++i) {      //Читаем массив пикселей
        for (int j = 0; j < bmp->InfoHeader.Width + (3*bmp->InfoHeader.Width % 4); ++j) {
            fread(&bmp->arr[i][j], 1, 1, file);
        }
    }

    int closeError = fclose(file);   //Закрываем поток

    if (closeError != 0 ) {     //если не получилось закрыть файл
        printf("\nERROR: COULD\'T CLOSE THE FILE NAMED \"%s\"\n\n", fileName);
        return NULL;
    }

    return bmp;
}

void Free(BMP *bmp, char** array, unsigned int width, unsigned int height) {    //Освобождаем память для всех указателей
    for (int i = 0; i < height; ++i) {  //Освобождение памяти для массива пикселей
        free(array[i]);
    }
    free(array);

    free(bmp);  //Освобождение памяти для структуры заголовков
}

long long Logic(BMP *bmp) {
    unsigned char ArrNew[bmp->InfoHeader.Height][bmp->InfoHeader.Width];    //Массив новых пикселей

    short flag = 0; //Флаг станет 1 если вселенная игры хоть как-то изменилась

    for (int i = bmp->InfoHeader.Height - 1; i >= 0; --i) {
        for (int j = 0; j < bmp->InfoHeader.Width; ++j) {
            //---Подсчет количества "живых" соседних клеток
            unsigned int count = 0;

            if (i > 0) {
                if ((int)bmp->arr[i - 1][j] == 0) count++;

                if (j > 0) {
                    if ((int)bmp->arr[i - 1][j - 1] == 0) count++;
                }

                if (j + 1 < bmp->InfoHeader.Width) {
                    if ((int)bmp->arr[i - 1][j + 1] == 0) count++;
                }
            }

            if (i + 1 < bmp->InfoHeader.Height) {
                if ((int)bmp->arr[i + 1][j] == 0) count++;

                if (j > 0) {
                    if ((int)bmp->arr[i + 1][j - 1] == 0) count++;
                }

                if (j + 1 < bmp->InfoHeader.Width) {
                    if ((int)bmp->arr[i + 1][j + 1] == 0) count++;
                }
            }

            if (j > 0) {
                if ((int)bmp->arr[i][j - 1] == 0) count++;
            }

            if (j + 1 < bmp->InfoHeader.Width) {
                if ((int)bmp->arr[i][j + 1] == 0) count++;
            }
            //---Конец подсчета

            if ((int)bmp->arr[i][j] == 0) {     //Условия выживания или вымирания клеток
                if (count < 2 || count > 3) {
                    ArrNew[i][j] = -1;
                    flag = 1;
                } else ArrNew[i][j] = bmp->arr[i][j];

            } else {
                if (count == 3) {
                    ArrNew[i][j] = 0;
                    flag = 1;
                } else ArrNew[i][j] = bmp->arr[i][j];
            }
        }
    }

    unsigned long long pointCount = 0;      //Колличество живых клеток

    for (int i = bmp->InfoHeader.Height - 1; i >= 0; --i) {     //Записываем в массив пикселей новые значения
        for (int j = 0; j < bmp->InfoHeader.Width; ++j) {
            if (bmp->arr[i][j] != ArrNew[i][j]) {
                bmp->arr[i][j] = ArrNew[i][j];
            }

            if ((int)ArrNew[i][j] == 0) pointCount++;   //Подсчет живых клеток
        }
    }
    if (flag) {     //Если картинка хоть как-то изменилась то выводим количиство пикслей
        return pointCount;
    } else return -1;    //Иначе выводим ноль, для завершения генерации
}

short PrintGen(BMP *bmp, char* dirName, int index) { //Запись поколения в бмп файл
    char fileName[100] = "";
    strcpy(fileName, dirName);  //В этих строчках просто формируем путь,название файла, расширение файла в одной строке
    strcat(fileName,"/");
    char indexStr[10];
    sprintf(indexStr, "%d", index);
    strcat(fileName, indexStr);
    strcat(fileName,".bmp");

    FILE *file = fopen(fileName, "wb");   //Открываем поток на запись

    if (file == NULL) {     //если файл не удалось открыть
        printf("\nERROR: COULD\'T CREATE THE FILE NAMED \"%s\"\n\n", fileName);
        return 1;
    }

    fwrite(&bmp->fileHeader, sizeof(BitMapFileHeader), 1, file);    //Последовательно записываем каждую часть структуры BMP
    fwrite(&bmp->InfoHeader, sizeof(BitMapInfoHeader), 1, file);
    fwrite(&bmp->ColorTable, sizeof(bmp->ColorTable), 1, file);
    for (int i = 0; i < bmp->InfoHeader.Height; ++i) {      //Запись массива пикселей в файл
        for (int j = 0; j < bmp->InfoHeader.Width + (3*bmp->InfoHeader.Width % 4); ++j) {
            fwrite(&bmp->arr[i][j], 1, 1,file);
        }
    }
    int closeError = fclose(file);   //Закрываем поток

    if (closeError != 0 ) {     //если не получилось закрыть файл
        printf("\nERROR: COULD\'T CLOSE THE FILE NAMED \"%s\"\n\n", fileName);
        return 1;
    }
    return 0;
}

int main(int argc, char **argv) {

    char fileName[50] = "";     //Имя входного файла
    char dirName[50] = "";  //Имя директироии
    unsigned long long maxIter = 0;     //Максимальное число генераций поколений
    int dumpFreq = 1;   //Частота сохранения поколений
    for (int i = 1; i < argc; i += 2) {     //Анализируем введенные аргументы
        if (!memcmp(argv[i], "--input", strlen(argv[i]))) {
            strcpy(fileName, argv[i + 1]);

        } else if (!memcmp(argv[i], "--output", strlen(argv[i]))) {
            strcpy(dirName, argv[i + 1]);

        } else if (!memcmp(argv[i], "--max_iter", strlen(argv[i]))) {
            maxIter = atoi(argv[i + 1]);     //преобразуем строу в число

        } else if (!memcmp(argv[i], "--dump_freq", strlen(argv[i]))) {
            dumpFreq = atoi(argv[i + 1]);    //То же самое

        } else {
            printf("\nERROR: INCORRECT INPUT FORMAT\n\n");
        }
    }

    BMP* bmp = ReadBMPFile(fileName);

    if (bmp == NULL) return 1;

    unsigned long long pointCount = 1;      //Количество "живых" клеток
    unsigned long long count = 0;   //Количество уже сгенерированных поколений

    while (pointCount > 0) {
        pointCount = Logic(bmp);
        if (pointCount == -1) break;     //Если живых клеток нет конец генерации

        if (count % (dumpFreq + 0) == 0) {    //Запись поколений с требуемой частотой
            short printError = PrintGen(bmp, dirName, count + 1);
            if (printError == 1) {
                return 1;
            }
        }
        count++;

        if (count == maxIter) break;    //Если количестов генераций равно количеству требуемых конец генерации
    }
    printf("\nSUCCESSFULLY\n\n");
    Free(bmp, bmp->arr, bmp->InfoHeader.Width, bmp->InfoHeader.Height); //Освобожжение выделенной памяти
    return 0;
 }
