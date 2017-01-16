#include "httpfs.h"

int httpfs_chmod( const char *path ,
                  mode_t mode )
{
    // Read only FS!
    HTTPFS_RETURN( EACCES );
}
