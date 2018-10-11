project( httpfs )

# CMake setup
cmake_minimum_required( VERSION 2.8 )
find_package( PkgConfig )

# sources
file( GLOB fuse_api lib/fuse_api/*.cc )
file( GLOB lib_src lib/*.cc )
file( GLOB templates RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/lib/templates/ lib/templates/* )

# process templates
set( templates_h "" )
foreach( template ${templates} )
set( template_h ${template}.h )
list( APPEND templates_h ${template_h} )
add_custom_command(
  OUTPUT ${template_h}
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/lib/templates/${template} ${template}
  COMMAND xxd -i ${template} ${template_h}
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/lib/templates/${template}
)
endforeach( template )
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

# external libraries
find_package( CURL )
pkg_check_modules( FUSE REQUIRED fuse )
add_definitions( ${FUSE_CFLAGS} -DFUSE_USE_VERSION=29 -std=c++17 )
#add_definitions( ${EXTRADBG_CFLAGS} -fsanitize=address )

# httpfs library
add_library( httpfs STATIC ${lib_src} ${fuse_api} ${templates_h} )

# Other libs
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# compilation
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/lib/ )
add_executable( httpfs-bin main.cc )
set_target_properties( httpfs-bin PROPERTIES OUTPUT_NAME httpfs)
add_definitions( -Wall )
target_link_libraries( httpfs-bin httpfs Threads::Threads ${CURL_LIBRARY} ${FUSE_LIBRARIES} )

install( PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/httpfs DESTINATION bin )
