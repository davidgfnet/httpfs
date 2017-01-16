#include "httpfs.h"

int httpfs_rename( const char *path ,
                   const char *newpath )
{
    // Read only FS!
    HTTPFS_RETURN( EACCES );
}
