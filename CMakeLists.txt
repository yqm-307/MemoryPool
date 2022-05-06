cmake_minimum_required(VERSION 2.6)

project(yqmmalloc CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall")

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib) 
include_directories(${PROJECT_BINARY_DIR})

#源文件
set(ALLSRCS
    ./include/CentralCache.h
    ./include/FreeList.h
    ./include/PageCache.h
    ./include/SpanList.h
    ./include/ThreadCache.h
    ./include/Util.h
    ./include/ymalloc.h
    ./src/CentralCache.cpp
    ./src/PageCache.cpp
    ./src/ThreadCache.cpp
    #./src/ymalloc.cpp
)
#message("${ALLSRCS}")
add_library(yqmmalloc STATIC
    ${ALLSRCS}
)

target_link_libraries(yqmmalloc -lYUtil -lpthread)
