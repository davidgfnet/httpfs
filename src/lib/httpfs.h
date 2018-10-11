#ifndef _HTTPFS_HTTPFS_H
#define _HTTPFS_HTTPFS_H

/* common includes for API implementation */
#include <fuse.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>
#include <list>
#include <unordered_map>
#include <shared_mutex>
#include "debug.h"
#include "net.h"
#include "lrucache.h"

/* convenience macros used to implement the FUSE API functions; 'response' is
   the data received and it's available to the implementation; a structure named
   'header' may be filled with values to be passed to the PHP script before
   calling this macros; a 'struct raw_data raw_data' may contains the additional
   data to pass to the PHP script; this macros expect a following block where
   the logic should be implemented */
#define HTTPFS_DO_REQUEST( doget, path, offset, size ) \
    _HTTPFS_DO_REQUEST(doget, path, offset, size , ((httpfs*)fuse_get_context()->private_data))


/* common */
#define _HTTPFS_DO_REQUEST(doget, path, offset, size , httpfs ) \
    CURL *curl = curl_easy_init(); \
    LOGF( "REQUEST: %s ", path ); \
    struct raw_data _out = { 0 } , _header_data = { 0 }; \
    struct raw_data response = { 0 }; \
    unsigned content_length = 0; \
    long http_code = 0; \
    ( void )response; \
    ( void )_header_data; \
    ( void ) content_length; \
    ( void ) http_code; \
    if ( CURLE_OK != httpfs_do_get( curl, httpfs, doget, path, offset, size, &_out ) ) { \
        LOG( "REQUEST: failed" ); \
        HTTPFS_CLEANUP; \
        return -ECOMM; \
    } else

/* return errno from FUSE callback functions */
#define HTTPFS_RETURN( errno ) \
    LOGF( "RETURN: %s (%i)" , strerror( errno ) , errno );  \
    return -errno;

/* check the response status and return if an error is occurred */
#define ___HTTPFS_CHECK_RESPONSE_STATUS \
    response.payload = _out.payload + 1; \
    response.size = _out.size - 1; \
    DUMP_RAW_DATA( "RECEIVED " , _out ); \
    switch ( *_out.payload ) { \
    _HTTPFS_CHECK_HANDLE_ERROR( ENTRY_NOT_FOUND , ENOENT ) \
    _HTTPFS_CHECK_HANDLE_ERROR( CANNOT_ACCESS , EACCES ) \
    _HTTPFS_CHECK_HANDLE_ERROR( NOT_PERMITTED , EPERM ) \
    case HTTPFS_STATUS_OK: _HTTPFS_DUMP_STATUS; break; \
    default: HTTPFS_CLEANUP; HTTPFS_RETURN( EBADMSG ); \
    }

#define HTTPFS_CHECK_RESPONSE_STATUS \
    response.payload = _out.payload; \
    response.size = _out.size; \
    response.header = _out.header; \
    response.hsize = _out.hsize; \
    double tmp; \
    if (CURLE_OK == curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &tmp)) \
        content_length = tmp; \
    curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);

#define _HTTPFS_CHECK_HANDLE_ERROR( status , errno ) \
    case HTTPFS_STATUS_##status: \
    _HTTPFS_DUMP_STATUS; HTTPFS_CLEANUP; \
    HTTPFS_RETURN( errno )

/* to be called before return in FUSE API functions */
#define HTTPFS_CLEANUP \
    free( _out.payload ); \
    curl_easy_cleanup(curl);

/* initialization errors */
enum {
    HTTPFS_NO_ERROR ,
    HTTPFS_FUSE_ERROR ,
    HTTPFS_CURL_ERROR ,
    HTTPFS_UNREACHABLE_SERVER_ERROR ,
    HTTPFS_WRONG_CHROOT_ERROR ,
    HTTPFS_ERRNO_ERROR
};

/* context */
class httpfs {
public:
    httpfs(int cache_size)
      // Use 25% of cache size for metadata
     : enable_caching(cache_size > 0),
       cache_metadata(0.9f * cache_size / 4 / (sizeof(struct stat) + 64),
                             cache_size / 4 / (sizeof(struct stat) + 64)) {
    }

    const char *url;
    const char *remote_chroot;

    // Caching structures
    bool enable_caching;
    lru11::Cache<std::string, struct stat> cache_metadata;
    std::shared_mutex cache_metadata_mutex;
};

/* response status */
#define _( x ) HTTPFS_STATUS_##x ,
enum {
#include "statuses.def"
};
extern const char *HTTPFS_STATUS_NAMES[];

int httpfs_fuse_start( struct httpfs *httpfs ,
                       const char *url ,
                       char *mount_point );

#endif
