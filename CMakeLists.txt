cmake_minimum_required(VERSION 3.0)

project(FM-Index)

add_executable(bitvector src/main.cpp src/BitVector.cpp src/RankSupport.cpp src/SelectSupport.cpp)

# Include the directory where MyLibrary project is. Otherwise, we can't use the
# library
#add_subdirectory(include)

# Link the executable and the library together
#target_link_libraries(Project MyLibrary)



