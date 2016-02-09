cmake_minimum_required(VERSION 3.2)
project(Libccnx-common)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

if( UNIX )
  link_libraries(m)
endif( UNIX )

include( CTest )
include( version )
include( detectCacheSize )

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_C_FLAGS_NOPANTS "${CMAKE_C_FLAGS_NOPANTS} -O3 -DNDEBUG -DLibccnx_DISABLE_VALIDATION")

link_directories($ENV{CCNX_DEPENDENCIES}/lib)
include_directories($ENV{CCNX_DEPENDENCIES}/include)
set(OPENSSL_ROOT_DIR $ENV{CCNX_DEPENDENCIES})

find_package( LongBow REQUIRED )
include_directories(${LONGBOW_INCLUDE_DIRS})

find_package( LibEvent REQUIRED )
include_directories(${LIBEVENT_INCLUDE_DIRS})

find_package( Libparc REQUIRED )
include_directories(${LIBPARC_INCLUDE_DIRS})

find_package ( Threads REQUIRED )

find_package ( OpenSSL REQUIRED )

include_directories(${PROJECT_BINARY_DIR}/ccnx/common ${PROJECT_SOURCE_DIR})

find_package( Doxygen )

macro(AddTest testFile)
  add_executable(${ARGV0} ${ARGV0}.c)
  target_link_libraries(${ARGV0} ${LONGBOW_LIBRARIES})
  target_link_libraries(${ARGV0} ccnx_common)
  target_link_libraries(${ARGV0} ${LIBEVENT_LIBRARIES})
  target_link_libraries(${ARGV0} ${LIBPARC_LIBRARIES})
  target_link_libraries(${ARGV0} ${OPENSSL_LIBRARIES})
  target_link_libraries(${ARGV0} ${CMAKE_THREAD_LIBS_INIT})
  add_test(${ARGV0} ${ARGV0})
endmacro(AddTest)



add_subdirectory(ccnx/common)
#add_subdirectory(documentation)
