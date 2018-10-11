#include "httpfs.h"

int httpfs_chown( const char *path ,
                  uid_t uid ,
                  gid_t gid )
{
    // Read only FS!
    HTTPFS_RETURN(EACCES);
}
