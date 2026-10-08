#include "FileFormat.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

_Static_assert(sizeof(BareHeader) == 16,
               "Header must be 16 bytes");
_Static_assert(sizeof(PackageEntry) == 20,
               "Entry must be 20 bytes");

typedef struct {
    uint32_t id;
    const char *text;
} TextEntry;

/* Returns 0 on success, 1 on failure. */
int write_package(FILE* fp,
                  const TextEntry *entries,
                  uint32_t count)
{
    if (fp == NULL || (count != 0 && entries == NULL)) {
        return 1;
    }

    uint64_t payload_start =
        sizeof(BareHeader) +
        (uint64_t)count * sizeof(PackageEntry);

    /* Validate inputs and total size before opening the file. */
    uint64_t total_size = payload_start;

    for (uint32_t i = 0; i < count; ++i) {
        if (entries[i].text == NULL) {
            return 1;
        }

        uint64_t size = (uint64_t)strlen(entries[i].text);

        if (size > UINT64_MAX - total_size) {
            return 1;
        }

        total_size += size;
    }

    if (fp == NULL) {
        return 1;
    }

    BareHeader header = {
        .magic = {'A', 'P', 'K', 'G'},
        .entry_count = count,
        .toc_offset = sizeof(BareHeader)
    };

    if (fwrite(&header, sizeof(header), 1, fp) != 1) {
        goto fail;
    }

    /* Write metadata describing where the text will go. */
    uint64_t offset = payload_start;

    for (uint32_t i = 0; i < count; ++i) {
        PackageEntry entry = {
            .id = entries[i].id,
            .file_offset = offset,
            .size = (uint64_t)strlen(entries[i].text)
        };

        if (fwrite(&entry, sizeof(entry), 1, fp) != 1) {
            goto fail;
        }

        offset += entry.size;
    }

    /* Write text in the same order as the table entries. */
    for (uint32_t i = 0; i < count; ++i) {
        size_t size = strlen(entries[i].text);

        if (fwrite(entries[i].text, 1, size, fp) != size) {
            goto fail;
        }
    }

    /* Closing can report a buffered write failure. */
    return 0;

fail:
    fclose(fp);
    return 1;
}
