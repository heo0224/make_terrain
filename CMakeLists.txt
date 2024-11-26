cmake_minimum_required(VERSION 3.13)
cmake_policy(VERSION 3.13)

set(PROJECT_NAME "make_terrain")
set(CMAKE_CXX_STANDARD 17)

set(WINDOW_NAME "make_terrain")
set(WINDOW_WIDTH 1280)
set(WINDOW_HEIGHT 720)

# define project name and specify the language as C and C++
project(${PROJECT_NAME} C CXX)

# specify the files to compile
file(GLOB SOURCES
  ${CMAKE_SOURCE_DIR}/src/*.cpp
  ${CMAKE_SOURCE_DIR}/includes/*.h
)
add_executable(${PROJECT_NAME} ${SOURCES})

# include 'Dependency.cmake' file to add dependencies
include(Dependency.cmake)

# sepecify the include directories (header files)
# and the link targets and their directories
if(UNIX)
  find_package(Threads REQUIRED)
  find_package(PkgConfig REQUIRED)
endif(UNIX)
target_include_directories(${PROJECT_NAME} PUBLIC ${DEP_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/includes)
target_link_directories(${PROJECT_NAME} PUBLIC ${DEP_LIB_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${DEP_LIBS} ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})

# define the preprocessor definitions
target_compile_definitions(${PROJECT_NAME} PUBLIC
    WINDOW_NAME="${WINDOW_NAME}"
    WINDOW_WIDTH=${WINDOW_WIDTH}
    WINDOW_HEIGHT=${WINDOW_HEIGHT}
    )

# ensure dependencies are built first
add_dependencies(${PROJECT_NAME} ${DEP_LIST})