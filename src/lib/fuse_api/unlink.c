#include "httpfs.h"

int httpfs_unlink( const char *path )
{
    // Read only FS!
    HTTPFS_RETURN( EACCES );
}
