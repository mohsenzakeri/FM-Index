cmake_minimum_required(VERSION 3.0)

include_directories(include)

project(bitvector)

set(bitvector_lib_srcs src/BitVector.cpp src/RankSupport.cpp src/SelectSupport.cpp src/FMIndex.cpp)
add_library(bitvector STATIC ${bitvector_lib_srcs})
target_include_directories(bitvector PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../include")

#add_executable(main src/main.cpp)
add_executable(benchmark_rank src/benchmark_rank.cpp)
add_executable(benchmark_select src/benchmark_select.cpp)
add_executable(benchmark_fmindex src/benchmark_fmindex.cpp)
add_executable(bwocc src/bwoccurance.cpp)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../include/libdivsufsort"
                    "${CMAKE_CURRENT_BINARY_DIR}/../include/libdivsufsort")
link_directories("${CMAKE_CURRENT_BINARY_DIR}/../include/libdivsufsortlib")

# Link the executable and the library together
target_link_libraries(benchmark_rank bitvector)
target_link_libraries(benchmark_select bitvector)
target_link_libraries(benchmark_fmindex bitvector divsufsort)
target_link_libraries(bwocc bitvector divsufsort)



