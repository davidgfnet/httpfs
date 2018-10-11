#include <unistd.h>
#include "httpfs.h"

const char * directory_header = "X-Directory";

#define FNV_PRIME_32 16777619
#define FNV_OFFSET_32 2166136261U

uint32_t FNV32(const char *s) {
    uint32_t hash = FNV_OFFSET_32, i;
    for(i = 0; i < strlen(s); i++) {
        hash = hash ^ (s[i]); // xor next byte into the bottom of the hash
        hash = hash * FNV_PRIME_32; // Multiply by prime number found to work well
    }
    return hash;
} 

int httpfs_getattr(const char *path, struct stat *stbuf) {
    // Check cached data first
    httpfs *ctx = ((httpfs*)fuse_get_context()->private_data);
    if (ctx->enable_caching) {
        // Read lock and lookup
        std::shared_lock<std::shared_mutex> lock(ctx->cache_metadata_mutex);
        if (ctx->cache_metadata.lookup(path, *stbuf))
            return 0;
    }

    HTTPFS_DO_REQUEST(0, path, 0, 0) {
        HTTPFS_CHECK_RESPONSE_STATUS;

        if (http_code == 404) {
            HTTPFS_CLEANUP;
            HTTPFS_RETURN(ENOENT);
        }

        response.header[response.hsize] = 0;
        unsigned isdir = strstr(response.header, directory_header) != 0;

        const unsigned blksize = 32*1024;  // Arbitrary
        unsigned filesize = content_length;

        memset(stbuf, 0, sizeof(struct stat));
        stbuf->st_dev = 1;
        stbuf->st_ino = FNV32(path);
        stbuf->st_mode = S_IRUSR | S_IRGRP | (isdir ? S_IFDIR : S_IFREG);
        stbuf->st_nlink = 1;
        stbuf->st_uid = getuid(); // Better cache this at mount time
        stbuf->st_gid = getgid();
        stbuf->st_rdev = 0;
        stbuf->st_size = filesize;
        stbuf->st_atime = 0;
        stbuf->st_mtime = 0;
        stbuf->st_ctime = 0;
        stbuf->st_blksize = blksize;
        stbuf->st_blocks = filesize / blksize;

        // Insert in cache
        if (ctx->enable_caching) {
            // Write lock + write
            std::unique_lock<std::shared_mutex> lock(ctx->cache_metadata_mutex);
            ctx->cache_metadata.add(path, *stbuf);
        }

        HTTPFS_CLEANUP;
        HTTPFS_RETURN(0);
    }
}
