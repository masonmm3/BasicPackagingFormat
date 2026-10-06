#include "stdint.h"

#pragma pack(push, 1)
typedef struct _bareHeader {
    char     magic[4];      // Bytes 0..3  -> "APKG"
    uint32_t entry_count;   // Bytes 4..7  -> 2
    uint64_t toc_offset;    // Bytes 8..15 -> 16
} BareHeader;               // Total = 16 contiguous bytes

typedef struct _packageEntry {
    uint32_t id;            // 1 = DLL, 2 = Model
    uint64_t file_offset;   // Byte position where raw data starts
    uint64_t size;          // Byte count of the raw data
} PackageEntry;
#pragma pack(pop)
