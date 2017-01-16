#include "httpfs.h"

int httpfs_mkdir( const char *path ,
                  mode_t mode )
{
    // Read only FS!
    HTTPFS_RETURN( EACCES );
}
