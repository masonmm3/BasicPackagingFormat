#include "stdio.h"
#include "FileFormat.h"
#include "stdlib.h"

errno_t read_header(FILE* fp, BareHeader* header);

errno_t read_toc(FILE* fp, long offset, PackageEntry* toc, size_t count);
