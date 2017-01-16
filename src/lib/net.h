#ifndef _HTTPFS_NET_H
#define _HTTPFS_NET_H

#include <curl/curl.h>

struct httpfs;

struct raw_data
{
    char *payload;
    size_t size;
    char *header;
    size_t hsize;
};

CURLcode httpfs_do_get(struct httpfs *httpfs, unsigned getbody, const char * path, unsigned offset, unsigned size, struct raw_data *out );

#endif
