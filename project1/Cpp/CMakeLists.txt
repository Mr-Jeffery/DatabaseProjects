
cmake_minimum_required(VERSION 3.10)
project(LoadData)

# Set C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)

# Set compiler flags
add_definitions(-O3)

# Add libpqxx subdirectory
add_subdirectory(lib/libpqxx)

add_executable(LoadData src/copy.cpp)
add_executable(SpeedTest src/speed.cpp)

# Link LoadData with libpqxx
target_link_libraries(LoadData PRIVATE pqxx)
target_link_libraries(SpeedTest PRIVATE pqxx)
# find_package(OpenMP)
# if(OpenMP_CXX_FOUND)
#     message("OpenMP found")
#     target_link_libraries(LoadData PUBLIC OpenMP::OpenMP_CXX)
# endif()