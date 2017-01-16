#include "httpfs.h"

int httpfs_create( const char *path ,
                   mode_t mode ,
                   struct fuse_file_info *fi )
{
    // Read only FS!
    HTTPFS_RETURN( EACCES );
}
