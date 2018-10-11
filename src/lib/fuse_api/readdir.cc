#include "httpfs.h"

unsigned strip_html(char * p, unsigned size) {
    unsigned i = 0;
    while (i < size) {
        if (p[i] == '<') {
            unsigned start = i;
            while (i < size && p[i] != '>') { i++; }
            unsigned end = ++i;

            memmove(&p[start], &p[end], size - end);
            size -= (end-start);

            i = start;
        }
        else
            i++;
    }
    return size;
}

int httpfs_readdir( const char *path ,
                    void *buf ,
                    fuse_fill_dir_t filler ,
                    off_t offset ,
                    struct fuse_file_info *fi )
{
     HTTPFS_DO_REQUEST(1, path, 0, 0) {
        HTTPFS_CHECK_RESPONSE_STATUS;
        if (http_code >= 400) {
            HTTPFS_CLEANUP;
            HTTPFS_RETURN( EBADF );
        }

        unsigned rsize = strip_html(response.payload, response.size);
        unsigned i = 0;
        while (i < rsize) {
            while (i < rsize && response.payload[i] == ' ') { i++; }

            if (i < response.size) {
                unsigned start = i;
                while (i < rsize && response.payload[i] != '\n') { i++; }
                unsigned end = i++;

                unsigned isdir = (response.payload[end-1] == '/');
                if (isdir)
                    end--;

                char fn[512];
                memcpy(fn, &response.payload[start], end-start);
                fn[end-start] = 0;

                struct stat fst;
                memset(&fst, 0, sizeof(fst));
                fst.st_ino = 0;
                fst.st_mode = S_IRUSR | S_IRGRP | (isdir ? S_IFDIR : S_IFREG);

                filler(buf, fn, &fst, 0);
            }
        }

        HTTPFS_CLEANUP;
        HTTPFS_RETURN(0);
    }
}
