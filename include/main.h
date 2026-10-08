#include "stdio.h"
#include "writer.h"
#include "stdlib.h"

errno_t read_header(FILE* fp, BareHeader* header);

errno_t read_toc(FILE* fp, long offset, PackageEntry* toc, size_t count);

errno_t run_package(FILE* fp);

errno_t write_cmd_package(FILE* path, char* text);

typedef enum _mode {
    run,
    write
}mode;
