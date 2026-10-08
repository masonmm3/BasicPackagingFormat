#include "main.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    FILE* fp;
    char* path = "../example.eaft";
    mode mode;
    char* fileMode = "rb";

    if (argc == 2 && strcmp(argv[1], "--run") == 0) {
        mode = run;
        fileMode = "rb";
    } else if (argc == 3 && strcmp(argv[1], "--write") == 0) {
        mode = write;
        fileMode = "wb";
    } else if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        printf("\f Possible Inputs\n--write\n   save an entry to the eaft\n--run\n   run the application with the current package format\n");
    } else {
        printf("bad command");
        return 1;
    }

    errno_t err = fopen_s(&fp, path, fileMode);
    if (err != 0 || fp == NULL) {
        printf("Failed to open file: %d\n", err);
        return 1;
    }


    switch (mode) {
        case run:
            run_package(fp);
            break;
        case write:
            char *text = argv[2];
            write_cmd_package(fp, text);
            break;
        default:
            return 0;
            break;
    }

    fclose(fp);
    return 0;
}

errno_t write_cmd_package(FILE* path, char* text) {
    if (text[0] == '\0') {
        fprintf(stderr, "Provide at least one text entry\n");
        return 1;
    }

    size_t count = 1;

    for (size_t i = 0; text[i] != '\0'; ++i) {
        if (text[i] == ',') {
            if (i == 0 || text[i + 1] == ',' ||
                text[i + 1] == '\0') {
                fprintf(stderr, "Text entries cannot be empty\n");
                return 1;
            }

            ++count;
        }
    }

    if (count > UINT32_MAX ||
        count > SIZE_MAX / sizeof(TextEntry)) {
        fprintf(stderr, "Too many entries\n");
        return 1;
    }

    TextEntry *entries = malloc(count * sizeof(*entries));
    if (entries == NULL) {
        fprintf(stderr, "Failed to allocate entries\n");
        return 1;
    }

    size_t index = 0;
    entries[0] = (TextEntry){1, text};

    for (char *p = text; *p != '\0'; ++p) {
        if (*p == ',') {
            *p = '\0';
            ++index;

            entries[index] = (TextEntry){
                .id = (uint32_t)(index + 1),
                .text = p + 1
            };
        }
    }

    int result = write_package(
        path, entries, (uint32_t)count
    );

    free(entries);

    if (result != 0) {
        fprintf(stderr, "Failed to write package\n");
    }

    return result;
}

errno_t run_package(FILE* fp) {
    BareHeader header;
    int err = read_header(fp, &header);
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

        printf("%s ", data);
        free(data);
    }

    free(toc);
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
