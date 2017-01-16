#include "httpfs.h"

int httpfs_rmdir( const char *path )
{
    // Read only FS!
    HTTPFS_RETURN( EACCES );
}
