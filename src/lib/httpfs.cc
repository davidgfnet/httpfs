#include "httpfs.h"
#include "fuse_api/fuse_api.h"

int httpfs_fuse_start(httpfs *ctx, const char *url, char *mount_point) {
    struct fuse_operations operations;
    memset(&operations, 0, sizeof(operations));
    #define _( x ) operations.x = &httpfs_##x;
    #include "fuse_functions.def"

    ctx->url = url;

    /* fuse arguments */
    int argc = 0;
    char *argv[4] = {0};
    argv[argc++] = "httpfs";
    if (HTTPFS_VERBOSE) argv[argc++] = "-d"; /* debug and core dump */
    argv[argc++] = mount_point;

    /* start loop */
    if (fuse_main(argc, argv, &operations, ctx))
        return HTTPFS_FUSE_ERROR;

    return HTTPFS_NO_ERROR;
}


