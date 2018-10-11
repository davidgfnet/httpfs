#include "httpfs.h"

int httpfs_truncate(const char *path, off_t offset) {
    // Read only FS!
    HTTPFS_RETURN(EACCES);
}
