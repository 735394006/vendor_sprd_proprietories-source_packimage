#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define ERR_EXIT(...) \
    do { fprintf(stderr, __VA_ARGS__); exit(1); } while (0)

static uint8_t* loadfile(const char* fn, size_t* num, size_t extra) {
    size_t n, j = 0; uint8_t* buf = 0;
    FILE* fi = fopen(fn, "rb");
    if (fi) {
        fseek(fi, 0, SEEK_END);
        n = ftell(fi);
        if (n) {
            fseek(fi, 0, SEEK_SET);
            buf = (uint8_t*)malloc(n + extra);
            if (buf) j = fread(buf, 1, n, fi);
        }
        fclose(fi);
    }
    if (num) *num = j;
    return buf;
}

int main(int argc, char** argv)
{
    if (argc < 2) ERR_EXIT("Usage: %s <filename>\n", argv[0]);

    char* filename = argv[1];
    uint8_t* mem; size_t size = 0;
    mem = loadfile(filename, &size, 0);
    if (!mem) ERR_EXIT("loadfile(\"%s\") failed\n", filename);

    if (!(*(uint64_t*)&mem[0x30])) ERR_EXIT("The file is not sprd trusted firmware\n");
    size = *(uint64_t*)&mem[0x30];
    printf("0x%x\n", size);

    int len = strlen(argv[1]);
    char* newFilename = (char*)malloc((len - 5) * sizeof(char));
    if (newFilename == NULL) {
        printf("Error: Memory allocation failed.\n");
        return 1;
    }
    strncpy(newFilename, filename, len - 9);  // ����ԭ�ļ�����ȥ��ԭ���ĺ�׺
    strncpy(newFilename + len - 9, ".bin", 4);  // �����µĺ�׺
    newFilename[len -5] = '\0';  // ����ַ���������
    mem += 0x200;
    FILE* file = fopen(newFilename, "wb");  // �Զ�����ģʽ���ļ�
    if (file == NULL) ERR_EXIT("Failed to create the file.\n");
    size_t bytes_written = fwrite(mem, sizeof(unsigned char), size, file);
    if (bytes_written != size) ERR_EXIT("Failed to write the file.\n");
    fclose(file);

    free(mem);

    return 0;
}
