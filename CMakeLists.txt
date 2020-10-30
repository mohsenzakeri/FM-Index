cmake_minimum_required(VERSION 3.0)

include_directories(include)

project(bitvector)

set(bitvector_lib_srcs src/BitVector.cpp src/RankSupport.cpp src/SelectSupport.cpp src/FMIndex.cpp)
add_library(bitvector STATIC ${bitvector_lib_srcs})
target_include_directories(bitvector PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../include")

#add_executable(main src/main.cpp)
add_executable(bwocc src/bwoccurance.cpp)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../libraries/libdivsufsort/include"
                    "${CMAKE_CURRENT_BINARY_DIR}/../libraries/libdivsufsort/include")
link_directories("${CMAKE_CURRENT_BINARY_DIR}/../libraries/libdivsufsort/lib")

# Link the executable and the library together
#target_link_libraries(main bitvector divsufsort)
target_link_libraries(bwocc bitvector divsufsort)



