#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define SeventhBite 128
#define SixthBite 64
#define FifthBite 32
#define FourthBite 16

#pragma push(1) //выравнивание структур по 1 байту

typedef struct {    //Заголовок mp3
    char marker[3]; //ID3
    unsigned char version[2];   //2.0 /3.0 /4.0
    unsigned char flags;     //флаги (7 бит - несинхронизация, 6 бит - наличие расширенного заголовка, 5 бит - "Эксперементальный", 4 бит - наличие нижнего заголовка)
    unsigned char sizeArr[4]; //каждый 7 бит нулевой, для коректности нужно вызвать функцию CorSize;
}MP3Header;

typedef struct {
    unsigned char sizeExHedArr[4];  //размер написан сконца на перед
    unsigned char flags[2];
    unsigned char sizePadArr[4];
}ExtendedHeader;

typedef struct {
    char ID[4];     //название фрейма
    unsigned char size[4];    //каждый 7 бит нулевой, для коректности нужно вызвать функцию CorSize;
    unsigned short flags;   //Два байта флагов
}FrameHeader;

typedef struct {
    FrameHeader frameHeader;
    unsigned int frameSize;
    char *frameData;
}Frame;

typedef struct {
    MP3Header header;
    unsigned int headerSize;
    ExtendedHeader exHeader;
    unsigned int exHeaderSize;
    Frame frame;
}MP3;

#pragma pop //конец выравнивания

unsigned int CorSizeSevenBytes(const unsigned char *size) {   //функция для получения корректного размера файла
    unsigned int sizeNew = 0;     //Убирает каждый 7 нулевой бит в каждом байте
    for (int i = 0; i < 4; ++i) {   //Проходим каждый из 4 байт размера и отбрасываем седьмой нулевой бит
        sizeNew += (unsigned int)((size[i]) * pow(2, 7* (3 - i)));
    }
    return sizeNew;
}

unsigned int CorSizeEightBytes(const unsigned char *size) {    //Записываем каждый из четырех байт с кон
    unsigned int sizeNew = 0;
    for (int i = 0; i < 4; ++i) {
        sizeNew += size[i] * pow(2, 8 * (3 - i));
    }
    return sizeNew;
}

void MakeNewSizeFrame(unsigned  char *sizeNew, unsigned int sizeOld) {
    unsigned char sizeOldArr[sizeof(sizeOld)];
    memcpy(sizeOldArr, &sizeOld, sizeof(sizeOld));
    for (int i = 0; i < 4; ++i) {
        sizeNew[i] = sizeOldArr[3-i];
    }
}

void PrintMP3Headers(MP3 mp3){
    printf("File identifier: %s\n", mp3.header.marker);
    printf("Version: %d.", mp3.header.version[0]);
    printf("%d\n", mp3.header.version[1]);
    printf("Flags: \n");
    printf("  Unsynchronisation: %d\n", mp3.header.flags & SeventhBite);
    printf("  Extended header: %d\n", mp3.header.flags & SixthBite);
    printf("  Experimental indicator: %d\n", mp3.header.flags & FifthBite);
    printf("  Footer present: %d\n", mp3.header.flags & FourthBite);
    printf("Size INFO: %d\n", mp3.headerSize);
}

void PrintFrame(MP3 mp3) {
    printf("\n%s: %s\n", mp3.frame.frameHeader.ID, mp3.frame.frameData);
    printf("Frame size: %d\n", mp3.frame.frameSize);
}

void ReadHeaders(FILE *file, MP3 *mp3) {
    fread(&mp3->header, sizeof(MP3Header), 1, file);
    mp3->headerSize = CorSizeSevenBytes(mp3->header.sizeArr);  //вычисляем размер метаданных

    if (mp3->header.flags & SixthBite) {
        fread(&mp3->exHeader, sizeof(ExtendedHeader), 1, file);
    }
}

void ShowMP3(char *filepath) {
    MP3 mp3;
    FILE *file = fopen(filepath, "rb"); //Открываем поток на чтение

    ReadHeaders(file, &mp3);
    PrintMP3Headers(mp3);     //выводим в консоль информацию заголовков

    while (ftell(file) <= mp3.headerSize) {
        fread(&mp3.frame.frameHeader, sizeof(FrameHeader), 1, file);

        if ((int)mp3.frame.frameHeader.ID[0] == 0) break;

        mp3.frame.frameSize = CorSizeEightBytes(mp3.frame.frameHeader.size);    //Размер данных фрейм
        mp3.frame.frameData = malloc(mp3.frame.frameSize);

        fgetc(file);    //Пробел перед строкой фрейма
        int k = 0;
        for (int j = 0; j < mp3.frame.frameSize - 1; ++j) {
            int ch = fgetc(file);
            if (ch >= 32 && ch <= 126) {
                fseek(file, ftell(file) - 1, SEEK_SET);
                fread(&mp3.frame.frameData[k], 1, 1, file);
                k++;
            }
        }

        PrintFrame(mp3); //Выводим информацию фрейма в консоль

        free(mp3.frame.frameData);    //Освобождаем память для повторного использования
    }
    fclose(file);
}

unsigned int FindFrame(FILE *file, MP3 mp3, char *frameName) {
    fseek(file, 0, SEEK_SET);
    ReadHeaders(file, &mp3);

    while (ftell(file) <= mp3.headerSize) {
        fread(&mp3.frame.frameHeader, sizeof(FrameHeader), 1, file);

        if ((int)mp3.frame.frameHeader.ID[0] == 0) return 0;

        mp3.frame.frameSize = CorSizeEightBytes(mp3.frame.frameHeader.size);    //Размер данных фрейма(без хедера)

        if (!memcmp(mp3.frame.frameHeader.ID, frameName, 4)) {
            unsigned int result = ftell(file) - sizeof(FrameHeader);
            //printf("%d\n", result);
            return result;
        } else {
            fseek(file, mp3.frame.frameSize, SEEK_CUR);
            //printf("%d\n", ftell(file));
        }
    }
    return 0;
}

void GetMP3(char *filepath, char *frameName) {
    MP3 mp3;
    FILE *file = fopen(filepath, "rb"); //Открываем поток на чтение

    int frameIndex = FindFrame(file, mp3, frameName);

    if (frameIndex != 0) {
        fseek(file, frameIndex, SEEK_SET);
        fread(&mp3.frame.frameHeader, sizeof(FrameHeader), 1, file);
        mp3.frame.frameSize = CorSizeEightBytes(mp3.frame.frameHeader.size);

        printf("\n%s: ", mp3.frame.frameHeader.ID);
        fgetc(file);    //Пробел перед каждой информацией фрейма
        for (int j = 0; j < mp3.frame.frameSize - 1; ++j) {
            int ch = fgetc(file);
            if (ch >= 32 && ch <= 126) {
                printf("%c", ch);
            }
        }
    }
    fclose(file);
}

void SetMP3(char *filepath, char *frameName, char *frameDataNew, int dataSize) {
    MP3 mp3;

    char *oldFile = "MODIFYING FILE.mp3";
    int renameError = rename(filepath, oldFile);  //Переименовываем файл (делаем его старым)

    if (renameError != 0) {
        printf("Program process was crushed (Rename Error)");     //если файл не переименуется программа завершится
        return;
    }

    FILE *oldF = fopen(oldFile, "rb"); //Открываем поток на чтение старого файла
    FILE *newF = fopen(filepath, "wb"); //Создаем файл с названием, которое было у старого

    int frameIndex = FindFrame(oldF, mp3, frameName);
        fseek(oldF, frameIndex, SEEK_SET);
        fread(&mp3.frame.frameHeader, sizeof(FrameHeader), 1, oldF);
        mp3.frame.frameSize = CorSizeEightBytes(mp3.frame.frameHeader.size);

        fseek(oldF, 0, SEEK_SET);
        fseek(newF, 0, SEEK_SET);
        while (feof(oldF) == 0) {
            char ch;

            fread(&ch, 1, 1, oldF);
            if (feof(oldF) != 0) break;
            fseek(oldF, -1, SEEK_CUR);

            if (ftell(oldF) != frameIndex) {
                fread(&ch, 1, 1, oldF);
                fwrite(&ch, 1, 1, newF);
            } else {
                MakeNewSizeFrame(mp3.frame.frameHeader.size, dataSize);

                fwrite(&mp3.frame.frameHeader, sizeof(FrameHeader), 1, newF);
                fseek(oldF, mp3.frame.frameSize + sizeof(FrameHeader), SEEK_CUR);
                for (int i = 0; i < dataSize; ++i) {    //Записываем новую информацию фрейма
                    fwrite(&frameDataNew[i], 1, 1, newF);
                }
            }
        }
    fclose(oldF);   //Закрытие всез потоков
    fclose(newF);
    remove(oldFile);    //Удаляем старый файл
}

int main(int argc, char **argv) {
    char filepath[100] = "";     //название анализируемого файла

    

   // strcat(filepath, argv[1]);    //получаем название файла из аргументов
    //ShowMP3("4.mp3");
    //GetMP3("2.mp3", "APIC");
    //rintf("%lu",sizeof(argv[1]));
    /*int i = 0;
    int count = 0;
    if (argc > 1) {
        while (argv[1][i] != '\0') {
            count++;
            i++;
        }
    }
    printf("%d\n",count);
    printf("%d", (int)' ');*/

    //ShowMP3("2.mp3");
    SetMP3("2.mp3", "TIT2", " B.I.D", 6);
    ShowMP3("2.mp3");


    //printf("%d",CorSizeEightBytes(ch2));

    return 0;
}
