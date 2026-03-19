#include "Header.h"

void writeFile(char *fileName, FType type, void *data, int noOfElements)
{
    FILE *file;

    switch (type)
    {
    case TEXT:
        file = fopen(fileName, "w");
        break;
    case BINARY:
        file = fopen(fileName, "wb");
        break;
    case TEXT_APPEND:
        file = fopen(fileName, "a");
        break;
    case BINARY_APPEND:
        file = fopen(fileName, "ab");
        break;
    }

    if (!file)
    {
        perror("File not found/corrupted");
        return;
    }

    if (type == TEXT)
    {
        char *message = (char *)data;
        fputs(message, file);

        printf("Data has been written to the file\n");
    }
    else
    {
        Bin *binData = (Bin *)data;

        for (int i = 0; i < noOfElements; i++)
        {
            size_t flag = fwrite(binData, sizeof(Bin), 1, file);
            binData++;
        }

        printf("Written successfully\n");
    }

    fclose(file);
}

void readFile(char *fileName, FType type)
{
    FILE *file = fopen(fileName, type == TEXT ? "r" : "rb");

    if (!file)
    {
        perror("File corrupted");
        return;
    }

    if (type == TEXT)
    {
        fseek(file, 0, SEEK_END);

        long fileSize = ftell(file);
        rewind(file);

        char *fileContents = malloc(fileSize + 1);

        fread(fileContents, 1, fileSize, file);
        fileContents[fileSize] = '\0';

        printf("%s\n", fileContents);

        free(fileContents);
        fclose(file);
    }
    else
    {
        fseek(file, 0, SEEK_END);

        long binFileSize = ftell(file);
        rewind(file);

        int structCount = binFileSize / sizeof(Bin);

        Bin *data = malloc(structCount * sizeof(Bin));
        fread(data, sizeof(Bin), structCount, file);

        for (Bin *bin = data; bin < (data + structCount); bin++)
            printf("Binary data for struct: %d\n", bin->value);

        free(data);
        fclose(file);
    }
}

void eraseBinData(char *fileName, int valueToErase)
{
    FILE *file = fopen(fileName, "rb");

    if (!file)
    {
        perror("File pointer doesnt exist");
        return;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    int structCounts = fileSize / sizeof(Bin);

    Bin *data = malloc(sizeof(Bin) * structCounts),
        *newData = malloc(sizeof(Bin) * structCounts);

    fread(data, sizeof(Bin), structCounts, file);

    int j = 0;
    for (int index = 0; index < structCounts; index++)
    {
        if (data[index].value == valueToErase)
            continue;

        newData[j++] = data[index];
    }

    writeFile(fileName, BINARY, newData, j);

    free(data);
    free(newData);
    fclose(file);
}

int main()
{
    char fileName[] = "TextC.txt",
         message[] = "This is a text file here";

    writeFile(fileName, TEXT, message, 0);
    readFile(fileName, TEXT);

    Bin data = {.value = 5},
        data2 = {.value = 10},
        data3 = {.value = 3};

    char binaryFileName[] = "Data.bin";

    writeFile(binaryFileName, BINARY, &data, 1);
    writeFile(binaryFileName, BINARY_APPEND, &data2, 1);
    writeFile(binaryFileName, BINARY_APPEND, &data3, 1);
    readFile(binaryFileName, BINARY);
    printf("\n");
    eraseBinData(binaryFileName, 10);
    readFile(binaryFileName, BINARY);

    return 0;
}