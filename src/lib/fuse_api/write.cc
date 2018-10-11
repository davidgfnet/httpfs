#include "httpfs.h"

int httpfs_write( const char *path ,
                  const char *buf ,
                  size_t size ,
                  off_t offset ,
                  struct fuse_file_info *fi )
{
    // Read only FS!
    HTTPFS_RETURN( EBADF );
}
