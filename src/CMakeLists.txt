cmake_minimum_required( VERSION 2.8 )
find_package( PkgConfig )

project( phpfs )

file( GLOB fuse_api fuse_api/*.c )
file( GLOB core *.c )
add_executable( phpfs ${core} ${fuse_api} )

add_definitions( -Wall )

find_package( CURL )
pkg_check_modules( FUSE REQUIRED fuse )
add_definitions( ${FUSE_CFLAGS} -DFUSE_USE_VERSION=29 )

target_link_libraries( phpfs ${CURL_LIBRARY} ${FUSE_LIBRARIES} )