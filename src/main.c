#include "main.h"
#include <stdio.h>
#include <string.h>

int main(void) {
    FILE* fp;
        errno_t err = fopen_s(&fp, "../example.eaft", "rb");
        if (err != 0 || fp == NULL) {
            printf("Failed to open file: %d\n", err);
            return 1;
        }

        BareHeader header;
        err = read_header(fp, &header);
        if (err != 0) {
            return 1;
        }

        if(strncmp("APKG", header.magic, 4) != 0) {
            printf("Invalid magic: %.4s\n", header.magic);
            return 1;
        }

        PackageEntry* toc = malloc(header.entry_count * sizeof(PackageEntry));
        if (toc == NULL) {
            printf("Failed to allocate TOC!\n");
            return 1;
        }
        err = read_toc(fp, (long)header.toc_offset, toc, header.entry_count);
        if (err != 0) {
            return 1;
        }

        for (uint32_t i = 0; i < header.entry_count; i++) {
            _fseeki64(fp, toc[i].file_offset, SEEK_SET);
            char* data = malloc(toc[i].size + 1);
            if (data == NULL) {
                printf("Failed to allocate data for entry %u!\n", i);
                free(toc);
                fclose(fp);
                return 1;
            }

            fread(data, 1, toc[i].size, fp);
            data[toc[i].size] = '\0'; // Make printable C string

            printf("%.5s ", data);
            free(data);
        }

        free(toc);
        fclose(fp);
        return 0;
}

errno_t read_header(FILE* fp, BareHeader* header) {
    fseek(fp, 0, SEEK_SET);
    if (fread(header, sizeof(BareHeader), 1, fp) != 1) {
        printf("Failed to read header!\n");
        fclose(fp);
        return 1;
    }
    return 0;
}

errno_t read_toc(FILE* fp, long offset, PackageEntry* toc, size_t count) {
    fseek(fp, offset, SEEK_SET);
    if (fread(toc, sizeof(PackageEntry), count, fp) != count) {
        printf("Failed to read TOC!\n");
        fclose(fp);
        return 1;
    }
    return 0;
}
