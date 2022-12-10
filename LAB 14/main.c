#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

//Макросы для битовых сдвигов и тд
#define SeventhBiteZero 127
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
    unsigned int frameSize;     //нормальный размер фрейма
    char *frameData;    //Данные фрейма
}Frame;

typedef struct {
    MP3Header header;
    unsigned int headerSize;    //Нормальный размер заголовка
    ExtendedHeader exHeader;
    unsigned int exHeaderSize;  //Дополнительный размер заголовка
    Frame frame;
}MP3;

#pragma pop //конец выравнивания

unsigned int CorSizeSevenBytes(const unsigned char *size) {   //функция для получения корректного размера заголовка
    unsigned int sizeNew = 0;     //Убирает каждый 7 нулевой бит в каждом байте
    for (int i = 0; i < 4; ++i) {   //Проходим каждый из 4 байт размера и отбрасываем седьмой нулевой бит
        sizeNew += (unsigned int)((size[i]) * pow(2, 7* (3 - i)));
    }
    return sizeNew;
}

unsigned int CorSizeEightBytes(const unsigned char *size) {    //Записываем каждый из четырех байт с конца
    unsigned int sizeNew = 0;
    for (int i = 0; i < 4; ++i) {
        sizeNew += (unsigned int)(size[i] * pow(2, 8 * (3 - i)));
    }
    return sizeNew;
}

void MakeNewSizeFrame(unsigned  char *sizeNew, unsigned int sizeOld) {    //Преобразует запись размера фрейма из нормального в который нужен
    unsigned char sizeOldArr[sizeof(sizeOld)];
    memcpy(sizeOldArr, &sizeOld, sizeof(sizeOld));
    for (int i = 0; i < 4; ++i) {
        sizeNew[i] = sizeOldArr[3-i];
    }
}

void MakeNewSizeHeader(unsigned  char *sizeNew, unsigned int sizeOld) {     //Преобразует запись размера заголовка из нормального в который нужен
    for (int i = 3; i >= 0; --i) {
        sizeNew[i] = sizeOld & SeventhBiteZero;
        sizeOld >>= 7;
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
    printf("Size: %d\n", mp3.frame.frameSize);
}

void ReadHeaders(FILE *file, MP3 *mp3) {    //чтение двух заголовков
    fseek(file, 0, SEEK_SET);
    fread(&mp3->header, sizeof(MP3Header), 1, file);    //главный хедер
    mp3->headerSize = CorSizeSevenBytes(mp3->header.sizeArr);  //вычисляем размер метаданных

    if (mp3->header.flags & SixthBite) {    //Проверка на существование дополнительного заголовка
        fread(&mp3->exHeader, sizeof(ExtendedHeader), 1, file);
    }
}

void ShowMP3(char *filepath) {
    MP3 mp3;
    FILE *file = fopen(filepath, "rb"); //Открываем поток на чтение

    if (file == NULL) {     //если файл не удалось открыть
        printf("\nERROR: COULD\'T OPEN THE FILE NAMED \"%s\"\n\n", filepath);
        return;
    }

    ReadHeaders(file, &mp3);    //Чтение заголовков
    PrintMP3Headers(mp3);     //выводим в консоль информацию заголовков

    while (ftell(file) <= mp3.headerSize) {     //пока не дойдем до конца метаданных читаем фреймы
        fread(&mp3.frame.frameHeader, sizeof(FrameHeader), 1, file);    //Чтение заголовка фрейма

        if ((int)mp3.frame.frameHeader.ID[0] == 0) break;   //Если вдруг заголовок равен нулю завершаем чтение

        mp3.frame.frameSize = CorSizeEightBytes(mp3.frame.frameHeader.size);    //Размер данных фрейма
        mp3.frame.frameData = malloc(mp3.frame.frameSize);

        fgetc(file);    //Пробел перед строкой фрейма

        int k = 0;
        for (int j = 0; j < mp3.frame.frameSize - 1; ++j) {     //Считываем только значимые символы фрейма
            int ch = fgetc(file);
            if (ch >= 32 && ch <= 126) {    //Проверка на вхождение символов в алфавит
                fseek(file, ftell(file) - 1, SEEK_SET);
                fread(&mp3.frame.frameData[k], 1, 1, file);     //Посимвольно записываем данные фрейма
                k++;
            }
        }

        PrintFrame(mp3); //Выводим информацию фрейма в консоль

        free(mp3.frame.frameData);    //Освобождаем память для повторного использования
    }
    int closeError = fclose(file);   //Закрываем поток

    if (closeError != 0 ) {     //если не получилось закрыть файл
        printf("\nERROR: COULD\'T CLOSE THE FILE NAMED \"%s\"\n\n", filepath);
        return;
    }
    printf("\n");
}

unsigned int FindFrame(FILE *file, MP3 mp3, char *frameName) {  //Нахождение положения указателя чтения в файле для фрейма по имени
    fseek(file, 0, SEEK_SET);
    ReadHeaders(file, &mp3);    //Чтение заголовков

    while (ftell(file) <= mp3.headerSize) {     //Читаем каждый фрейм
        fread(&mp3.frame.frameHeader, sizeof(FrameHeader), 1, file);    //Заголовок фрейма

        if ((int)mp3.frame.frameHeader.ID[0] == 0) return 0;    //Если заголовок равен нулю прекращаем чтение

        mp3.frame.frameSize = CorSizeEightBytes(mp3.frame.frameHeader.size);    //Размер данных фрейма

        if (!memcmp(mp3.frame.frameHeader.ID, frameName, strlen(frameName))) {  //Если нашли фрейм с нужным именем возращаем положение курсора
            unsigned int result = ftell(file) - sizeof(FrameHeader);
            return result;
        } else {
            fseek(file, mp3.frame.frameSize, SEEK_CUR);
        }
    }
    return 0;   //Если не нашли, возвращаем ноль
}

void GetMP3(char *filepath, char *frameName) {
    MP3 mp3;
    FILE *file = fopen(filepath, "rb"); //Открываем поток на чтение

    if (file == NULL) {     //если файл не удалось открыть
        printf("\nERROR: COULD\'T OPEN THE FILE NAMED \"%s\"\n\n", filepath);
        return;
    }

    int frameIndex = FindFrame(file, mp3, frameName);

    if (frameIndex != 0) {  //Если нужный фрейм существует принтим его в консоль
        fseek(file, frameIndex, SEEK_SET);
        fread(&mp3.frame.frameHeader, sizeof(FrameHeader), 1, file);
        mp3.frame.frameSize = CorSizeEightBytes(mp3.frame.frameHeader.size);

        printf("\n%s: ", mp3.frame.frameHeader.ID);
        fgetc(file);    //Пробел перед каждой информацией фрейма
        for (int j = 0; j < mp3.frame.frameSize - 1; ++j) {     //Выводим только читабильную часть фрейма
            int ch = fgetc(file);
            if (ch >= 32 && ch <= 126) {    //Проверка на фхождение в алфавит
                printf("%c", ch);
            }
        }
        printf("\n\n");
    } else {    //Если требуемого фрейма нет
        printf("\nERROR: THERE IS NO SUCH FRAME IN FILE NAMED \"%s\"\n\n", filepath);
    }

    int closeError = fclose(file);   //Закрываем поток

    if (closeError != 0 ) {     //если не получилось закрыть файл
        printf("\nERROR: COULD\'T CLOSE THE FILE NAMED \"%s\"\n\n", filepath);
        return;
    }
}

void SetMP3(char *filepath, char *frameName, char *frameDataNew, int dataSize) {
    MP3 mp3;

    char *oldFile = "MODIFYING FILE.mp3";
    int renameError = rename(filepath, oldFile);  //Переименовываем файл (делаем его старым)

    if (renameError != 0) {
        printf("\nERROR: FILE RENAMING ERROR\n\n");     //если файл не переименуется программа завершится с ошибкой
        return;
    }

    FILE *oldF = fopen(oldFile, "rb"); //Открываем поток на чтение старого файла
    FILE *newF = fopen(filepath, "wb"); //Создаем файл с названием, которое было у старого

    if (oldF == NULL || newF == NULL)  {     //если какой-то файл не удалось открыть
        printf("\nERROR: COULD\'T OPEN THE FILES NAMED \"%s\" \"%s\"\n\n", oldFile, filepath);
        return;
    }

    int frameIndex = FindFrame(oldF, mp3, frameName);

    ReadHeaders(oldF, &mp3);    //Чтение заголовков

    fseek(oldF, frameIndex, SEEK_SET);      //Выставляем в уже старом файле курсор на чтение с момента где начинается изменяемый фрейм
    fread(&mp3.frame.frameHeader, sizeof(FrameHeader), 1, oldF);  //Чтение заголовка фрейма из старого файла
    mp3.frame.frameSize = CorSizeEightBytes(mp3.frame.frameHeader.size);    //Вычисление нормального размера фрейма

    fseek(oldF, 0, SEEK_SET);   //Выставляем курсоры в новом и старом файле на начала файлов
    fseek(newF, 0, SEEK_SET);
    while (feof(oldF) == 0) {   //Пока старый файл не закончился
        char ch;

        fread(&ch, 1, 1, oldF);
        if (feof(oldF) != 0) break;     //Читаем один символ из старого файла и ловим символ конца файла
        fseek(oldF, -1, SEEK_CUR);

        if (ftell(oldF) == frameIndex) {    //если чтение в старом файле дошло до фрема, который мы изменяем
            MakeNewSizeFrame(mp3.frame.frameHeader.size, dataSize); //переделываем размер нового фрейма из нормального в тот который нужен

            fwrite(&mp3.frame.frameHeader, sizeof(FrameHeader), 1, newF);   //Записываем измененный заголовок фрейма в новый файл
            fseek(oldF, mp3.frame.frameSize + sizeof(FrameHeader), SEEK_CUR);   //передвигаем курсор чтения в старом файле на размер заголовка фрейма
            for (int i = 0; i < dataSize; ++i) {    //Записываем новую информацию фрейма в новый файл
                fwrite(&frameDataNew[i], 1, 1, newF);
            }

        } else if (ftell(oldF) == sizeof(MP3Header) - sizeof(mp3.header.sizeArr)) {   //Если чтение из старого файла дошло до размера всех метаданных в главном заголовке
            int newHeaderSize = mp3.headerSize + (dataSize - mp3.frame.frameSize);   //новый размер метаданных в человеческом виде
            MakeNewSizeHeader(mp3.header.sizeArr, newHeaderSize);   //Переделываем его в вид с 0 в седьмом бите для записи в файл

            fwrite(&mp3.header.sizeArr, sizeof(mp3.header.sizeArr), 1, newF);   //Записываем размер в новый файл
            fseek(oldF, sizeof(mp3.header.sizeArr), SEEK_CUR);  //передвигаем курсор чтения в старом файле

        } else {    //Переписываем последовательно каждый байт из старого файла в новый файл
            fread(&ch, 1, 1, oldF);
            fwrite(&ch, 1, 1, newF);
        }
    }
    printf("\nThe frame \"%s\" has been changed\n\n", frameName);

    short oldFCloseError = fclose(oldF);   //Закрытие всех потоков
    short newFCloseError = fclose(newF);

    if (oldFCloseError != 0 || newFCloseError != 0) {     //если не получилось закрыть какой-то из файлов
        printf("\nERROR: COULD\'T CLOSE THE FILES NAMED \"%s\" \"%s\"\n\n", oldFile, filepath);
        return;
    }

    short removeError = remove(oldFile);    //Удаляем старый файл

    if (removeError != 0) {
        printf("\nERROR: COULD\'T DELETE THE FILE NAMED \"%s\"\n\n", oldFile);  //Если файл не удалится программа завершится
        return;
    }
}

int main(int argc, char **argv) {
    char *filepath = NULL;  //название мп3 файла
    char *frameNameSet = NULL;  //Название фрейма, который надо изменить
    char *frameNameGet = NULL;  //Название фрейма, который надо вывести
    char *newFrameVal = NULL;   //Данные, которые надо записать в фрейм
    short showFlag = 0; //Флаги для обработки команд
    short setFlag = 0;
    short getFlag = 0;
    short valFlag = 0;
    short pathFlag = 0;

    for (int i = 1; i < argc; ++i) {    //Обработка аргументов командной строки
        char command[50] = "";

        int j;
        for (j = 0 ; j < strlen(argv[i]); ++j) {    //Чтение команды до символа "="
            if (argv[i][j] == '=') break;
            command[j] = argv[i][j];
        }

        if (strlen(argv[i]) - j > 0) {  //Обработка данных, которые ввел пользователь
            char *value;

            value = malloc(sizeof(value[0]) * strlen(argv[i]) - j);     //Читаем строку которая находится полсе "=" после команды
            strcpy(value, &argv[i][j + 1]);

            if (!memcmp(command, "--filepath", strlen(command))) {
                filepath = malloc(sizeof(filepath[0]) * strlen(value));
                strcpy(filepath, value);
                pathFlag = 1;

            } else if (!memcmp(command, "--set", strlen(command))) {
                frameNameSet = malloc(sizeof(frameNameSet[0]) * strlen(value));
                strcpy(frameNameSet, value);
                setFlag = 1;

            } else if (!memcmp(command, "--get", strlen(command))) {
                frameNameGet = malloc(sizeof(frameNameGet[0]) * strlen(value));
                strcpy(frameNameGet, value);
                getFlag = 1;

            } else if (!memcmp(command, "--value", strlen(command))) {
                newFrameVal = malloc(sizeof(newFrameVal[0]) * strlen(value) + 1);
                newFrameVal[0] = ' ';
                strcat(newFrameVal, value);
                valFlag = 1;
            }
            free(value);

        } else if (!memcmp(command, "--show", strlen(command))) {
            showFlag = 1;

        }
    }

    if (!pathFlag) {    //Если не ввели название файла
        printf("\nERROR: INCORRECT FILE NAME: \"%s\"\n\n", filepath);
    }

    if (showFlag) {     //Если требуется вывести метаданные
        ShowMP3(filepath);

    } else if (setFlag) {   //Если требуется изменить фрейм
        if (valFlag) {
            SetMP3(filepath, frameNameSet, newFrameVal, strlen(newFrameVal));
        } else {
            printf("\nERROR: INCORRECT NEW FRAME DATA FORMAT\n\n");
        }

    } else if (getFlag) {       //Если требуется вывести фрейм
        GetMP3(filepath, frameNameGet);

    }else printf("\nERROR: INCORRECT INPUT FORMAT\n\n");

    free(filepath);
    free(frameNameSet);
    free(frameNameGet);
    free(newFrameVal);
    return 0;
}
