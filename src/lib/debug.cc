#include <ctype.h>
#include "net.h"

int HTTPFS_VERBOSE = 1;

void httpfs_dump_raw_data(struct raw_data *raw_data) {
    fprintf(stderr , "'");
    for (unsigned i = 0 ; i < raw_data->size ; i++) {
        unsigned char c = raw_data->payload[i];
        fprintf( stderr , isprint( c ) ? "%c" : "\\x%02x" , c );
    }

    fprintf( stderr , "'\n" );
}
