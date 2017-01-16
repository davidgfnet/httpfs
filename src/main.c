#include <stdlib.h>
#include "httpfs.h"
#include "fuse_api/fuse_api.h"
#include "version.h"

static void usage()
{
    fprintf( stderr ,
             "Usage:\n\n"
             "    httpfs --help\n"
             "    httpfs --version\n"
             "    httpfs mount <url> <mount_point>\n" );
}

static void info()
{
    fprintf( stderr , "httpfs " HTTPFS_VERSION "\n" );
}

static void set_verbose_mode()
{
    char *env;

    env = getenv( "HTTPFS_VERBOSE" );
    if ( env && strcmp( env , "1" ) == 0 ) HTTPFS_VERBOSE = 1;
}

int main( int argc , char *argv[] )
{
    set_verbose_mode();

    if ( argc == 2 && strcmp( argv[ 1 ] , "--version" ) == 0 )
    {
        info();
    }
    else if ( argc == 2 && strcmp( argv[ 1 ] , "--help" ) == 0 )
    {
        usage();
    }
    else if ( argc == 4 && strcmp( argv[ 1 ] , "mount" ) == 0 )
    {
        struct httpfs httpfs;
        const char *url;
        char *mount_point;
        int rv;

        url = argv[ 2 ];
        mount_point = argv[ 3 ];

        rv = httpfs_fuse_start( &httpfs , url , mount_point );

        if ( rv )
        {
            fprintf( stderr , "Unable to mount: " );

            switch ( rv )
            {
            case HTTPFS_FUSE_ERROR:
                fprintf( stderr , "cannot initialize FUSE\n" );
                break;

            case HTTPFS_CURL_ERROR:
                fprintf( stderr , "cannot initialize cURL\n" );
                break;

            case HTTPFS_UNREACHABLE_SERVER_ERROR:
                fprintf( stderr , "cannot reach the remote server\n" );
                break;

            case HTTPFS_WRONG_CHROOT_ERROR:
                fprintf( stderr , "cannot find the remote path\n" );
                break;

            case HTTPFS_ERRNO_ERROR:
                fprintf( stderr , "errno (%i) %s\n" , errno , strerror( errno ) );
                break;
            }
        }
    }
    else
    {
        usage();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
