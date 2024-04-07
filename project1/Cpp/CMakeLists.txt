# cmake_minimum_required(VERSION 3.10)
# project(LoadData)

# set(CMAKE_CXX_STANDARD 20)


# add_executable(LoadData src/main.cpp)

# # Specify that LoadData depends on libpqxx
# # add_dependencies(LoadData libpqxx)

# # Find libpqxx
# find_package(libpqxx REQUIRED)
# # Link libpqxx to LoadData
# target_link_libraries(LoadData PRIVATE pqxx)
cmake_minimum_required(VERSION 3.10)
project(LoadData)

# Set C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

# Add libpqxx subdirectory
add_subdirectory(lib/libpqxx)

add_executable(LoadData src/main.cpp)

# Link LoadData with libpqxx
target_link_libraries(LoadData PRIVATE pqxx)
