cmake_minimum_required(VERSION 3.0)

project(FM-Index)

add_executable(bitvector src/main.cpp src/BitVector.cpp src/RankSupport.cpp src/SelectSupport.cpp src/FMIndex.cpp)

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
target_link_libraries(bitvector divsufsort)



