#include "httpfs.h"
#include "fuse_api/fuse_api.h"

static int check_remote_availability( struct httpfs *httpfs )
{
    return 0;
/*
    _HTTPFS_DO_REQUEST( "/", 0, 0, httpfs )
    {
        HTTPFS_CHECK_RESPONSE_STATUS;
        HTTPFS_CLEANUP;
        HTTPFS_RETURN( 0 );
    }*/
}

int httpfs_fuse_start( struct httpfs *httpfs ,
                       const char *url ,
                       char *mount_point )
{
    int argc;
    char *argv[ 4 ];
    int rv;

    const struct fuse_operations operations = {
#define _( x ) .x = httpfs_##x ,
#include "fuse_functions.def"
    };

    httpfs->url = url;

    /* initialize curl */
    httpfs->curl = curl_easy_init();
    if ( !httpfs->curl )
    {
        return HTTPFS_CURL_ERROR;
    }

    /* check remote availability before mounting */
    rv = -check_remote_availability( httpfs );
    switch ( rv )
    {
    case 0: break;
    case ECOMM: return HTTPFS_UNREACHABLE_SERVER_ERROR;
    case ENOENT: return HTTPFS_WRONG_CHROOT_ERROR;
    default:
        {
            errno = rv;
            return HTTPFS_ERRNO_ERROR;
        }
    }

    /* fuse arguments */
    argc = 0;
    argv[ argc++ ] = "httpfs";
    argv[ argc++ ] = "-s"; /* single thread */
    if ( HTTPFS_VERBOSE ) argv[ argc++ ] = "-d"; /* debug and core dump */
    argv[ argc++ ] = mount_point;

    /* start loop */
    if ( fuse_main( argc , argv , &operations , httpfs ) )
    {
        return HTTPFS_FUSE_ERROR;
    }

    return HTTPFS_NO_ERROR;
}


