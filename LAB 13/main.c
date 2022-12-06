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
    FILE* file = fopen(fileName, "r");  //Открываем поток на чтение

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

    fclose(file);   //Закрываем поток чтения
    return bmp;
}

/*void PixelInfo(BMP* fbmp) { //Вывод информации о бмп

    printf("%c%c\n",fbmp->fileHeader.Type[0], fbmp->fileHeader.Type[1]);
    printf("%d\n", fbmp->fileHeader.Size);
    printf("%d\n", fbmp->fileHeader.Reserved1);
    printf("%d\n", fbmp->fileHeader.OffBits);
    printf("%d\n", fbmp->InfoHeader.BitCount);
    printf("%d\n", fbmp->InfoHeader.Width);
    printf("%lu\n", sizeof(fbmp->ColorTable));

    for (int i = fbmp->InfoHeader.Height - 1; i >= 0; --i) {
        for (int j = 0; j < fbmp->InfoHeader.Width + (3*fbmp->InfoHeader.Width % 4); ++j) {
            printf("%d ", fbmp->arr[i][j]);
        }
        printf("\n");
    }
}*/

void Free(BMP *bmp, char** array, unsigned int width, unsigned int height) {    //Освобождаем память для всех указателей
    for (int i = 0; i < height; ++i) {  //Освобождение памяти для массива пикселей
        free(array[i]);
    }
    free(array);

    free(bmp);  //Освобождение памяти для структуры заголовков
}

unsigned long long Logic(BMP *bmp) {
    unsigned char ArrNew[bmp->InfoHeader.Height][bmp->InfoHeader.Width];

    short flag = 0;

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

    unsigned long long pointCount = 0;

    for (int i = bmp->InfoHeader.Height - 1; i >= 0; --i) {     //Записываем в массив пикселей новые значения
        for (int j = 0; j < bmp->InfoHeader.Width; ++j) {
            if (bmp->arr[i][j] != ArrNew[i][j]) {
                //flag = 1;
                bmp->arr[i][j] = ArrNew[i][j];
            }

            if ((int)ArrNew[i][j] == 0) pointCount++;
        }
    }
    if (flag) {
        return pointCount;
    } else return 0;
}

void PrintGen(BMP *bmp, char* dirName, int index) { //Запись поколения в бмп файл
    char fileName[100] = " ";
    strcpy(fileName, dirName);  //В этих строчках просто формируем путь,название файла, расширение файла в одной строке
    strcat(fileName,"/");
    char indexStr[10];
    sprintf(indexStr, "%d", index);
    strcat(fileName, indexStr);
    strcat(fileName,".bmp");

    FILE *file = fopen(fileName, "w");   //Открываем поток на запись
    fwrite(&bmp->fileHeader, sizeof(BitMapFileHeader), 1, file);    //Последовательно записываем каждую часть структуры BMP
    fwrite(&bmp->InfoHeader, sizeof(BitMapInfoHeader), 1, file);
    fwrite(&bmp->ColorTable, sizeof(bmp->ColorTable), 1, file);
    //fseek(file,bmp->fileHeader.OffBits, SEEK_SET);
    for (int i = 0; i < bmp->InfoHeader.Height; ++i) {      //Запись массива пикселей в файл
        for (int j = 0; j < bmp->InfoHeader.Width + (3*bmp->InfoHeader.Width % 4); ++j) {
            fwrite(&bmp->arr[i][j], 1, 1,file);
        }
    }
    fclose(file); //Закрываем поток
}

int main(int argc, char **argv) {

    char fileName[50] = "";
    char dirName[50] = "";
    unsigned long long maxIter = 0;
    int dumpFreq = 1;
    for (int i = 1; i < argc; i += 2) {
        if (argv[i][2] == 'i') strcpy(fileName, argv[i + 1]);
        if (argv[i][2] == 'o') strcpy(dirName, argv[i + 1]);
        if (argv[i][2] == 'm') maxIter = atoi(argv[i + 1]);
        if (argv[i][2] == 'd') dumpFreq = atoi(argv[i + 1]);
    }

    BMP* fbmp = ReadBMPFile(fileName);
    unsigned long long pointCount = 1;
    unsigned long long count = 0;

    while (pointCount > 0) {
        count++;
        pointCount = Logic(fbmp);
        if (pointCount == 0) break;
        PrintGen(fbmp, dirName, count);
        if (count == maxIter) break;
    }
    /*for (int i = 0; i < 256; ++i) {
        printf("%d ",fbmp->ColorTable[i]);
    }*/
    //PixelInfo(fbmp);
    Free(fbmp, fbmp->arr, fbmp->InfoHeader.Width, fbmp->InfoHeader.Height);
    return 0;
 }
