#include "httpfs.h"

int httpfs_read( const char *path ,
                 char *buf ,
                 size_t size ,
                 off_t offset ,
                 struct fuse_file_info *fi )
{
    HTTPFS_DO_REQUEST(1, path, offset, size )
    {
        HTTPFS_CHECK_RESPONSE_STATUS;
        if ( response.size > size || http_code >= 400)
        {
            HTTPFS_CLEANUP;
            HTTPFS_RETURN( EBADMSG );
        }

        memcpy( buf , response.payload , response.size );

        HTTPFS_CLEANUP;
        return response.size;
    }
}
