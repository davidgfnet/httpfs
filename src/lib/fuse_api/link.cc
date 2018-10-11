#include "httpfs.h"

int httpfs_link( const char *path ,
                 const char *newpath )
{
    // Read only FS!
    HTTPFS_RETURN( EACCES );
}
