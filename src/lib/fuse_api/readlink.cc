#include "httpfs.h"

int httpfs_readlink( const char *path ,
                     char *buf ,
                     size_t size )
{
    // Do not support symlinks
    HTTPFS_RETURN( EINVAL );
}
