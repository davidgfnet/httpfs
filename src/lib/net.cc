#include <stdlib.h>
#include <string.h>
#include "httpfs.h"

static size_t retrieve_chunk(char *ptr, size_t size, size_t nmemb, void *userdata) {
    struct raw_data *data = ( struct raw_data * )userdata;
    size_t chunk_size = size * nmemb;

    data->payload = (char*)realloc(data->payload , data->size + chunk_size + 1);
    memcpy(data->payload + data->size , ptr , chunk_size);
    data->size += chunk_size;

    return chunk_size;
}

static size_t retrieve_header(char *ptr, size_t size, size_t nmemb, void *userdata) {
    struct raw_data *data = ( struct raw_data * )userdata;
    size_t chunk_size = size * nmemb;

    chunk_size = size * nmemb;
    data = ( struct raw_data * )userdata;
    data->header = (char*)realloc(data->header, data->hsize + chunk_size + 1);
    memcpy(data->header + data->hsize, ptr, chunk_size);
    data->hsize += chunk_size;

    return chunk_size;
}

CURLcode httpfs_do_get(CURL *curl, struct httpfs *httpfs, unsigned getbody, const char * path, unsigned offset, unsigned size, struct raw_data *out) {
    out->payload = (char*)malloc(1);
    out->header = (char*)malloc(1);
    out->size = out->hsize = 0;

    char range[128];
    sprintf(range, "%u-%u", offset, offset+size-1);

    char fullurl[2048];
    strcpy(fullurl, httpfs->url);
    char * encpath = curl_easy_escape(curl, path, 0);
    strcat(fullurl, encpath);
    curl_free(encpath);

    curl_easy_setopt( curl , CURLOPT_URL , fullurl );

    //if (size > 0)
        curl_easy_setopt( curl , CURLOPT_RANGE, range);

    curl_easy_setopt(curl, CURLOPT_NOBODY, !getbody);

    //curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1);

    // Header
    curl_easy_setopt( curl , CURLOPT_HEADERFUNCTION , retrieve_header );
    curl_easy_setopt( curl , CURLOPT_HEADERDATA , out );

    // Body
    curl_easy_setopt( curl , CURLOPT_WRITEFUNCTION , retrieve_chunk );
    curl_easy_setopt( curl , CURLOPT_WRITEDATA , out );

    return curl_easy_perform( curl );
}
