cmake_minimum_required(VERSION 3.0)

include_directories(include)

project(bitvector)

#add_executable(bitvector src/main.cpp src/BitVector.cpp src/RankSupport.cpp src/SelectSupport.cpp src/FMIndex.cpp )

set(bitvector_lib_srcs src/BitVector.cpp src/RankSupport.cpp src/SelectSupport.cpp src/FMIndex.cpp)
add_library(bitvector STATIC ${bitvector_lib_srcs})
target_include_directories(bitvector PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../include")

#set(SOURCES ${YOUR_DIRECTORY}/file1.h ${YOUR_DIRECTORY}/file2.h)

add_executable(main src/main.cpp)

## Targets ##
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../libraries/libdivsufsort/include"
                    "${CMAKE_CURRENT_BINARY_DIR}/../libraries/libdivsufsort/include")
link_directories("${CMAKE_CURRENT_BINARY_DIR}/../libraries/libdivsufsort/lib")

#foreach(src main BitVector RankSupport SelectSupport)
#  add_executable(${src} src/${src}.cpp)
#  target_link_libraries(${src} divsufsort)
#endforeach(src)

# Include the directory where MyLibrary project is. Otherwise, we can't use the library
# add_subdirectory(libraries/libdivsufsort)

# Link the executable and the library together
target_link_libraries(main bitvector divsufsort)



