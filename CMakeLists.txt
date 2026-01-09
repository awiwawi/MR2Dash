cmake_minimum_required(VERSION 3.16)

if(POLICY CMP0194)
  cmake_policy(SET CMP0194 NEW)
endif()

if(POLICY CMP0169)
  cmake_policy(SET CMP0169 OLD)
endif()

project(MR2_Dash C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# --- Dependencies ---

# 1. Fetch LVGL
include(FetchContent)
FetchContent_Declare(
    lvgl
    GIT_REPOSITORY https://github.com/lvgl/lvgl.git
    GIT_TAG        v9.1.0
)
FetchContent_GetProperties(lvgl)
if(NOT lvgl_POPULATED)
    FetchContent_Populate(lvgl)
endif()

# 2. Find SDL2
find_package(SDL2 REQUIRED)

# --- Configuration ---
add_definitions(-DLV_CONF_INCLUDE_SIMPLE)
add_definitions(-DLV_LVGL_H_INCLUDE_SIMPLE)

# --- Sources ---
file(GLOB_RECURSE USER_SOURCES 
    "src/*.c" 
)

# Filter out broken font file if it exists
list(FILTER USER_SOURCES EXCLUDE REGEX "Fugaz.c")

file(GLOB_RECURSE LVGL_SOURCES 
    "${lvgl_SOURCE_DIR}/src/*.c"
)

# --- Executable ---
add_executable(${PROJECT_NAME} 
    ${USER_SOURCES} 
    ${LVGL_SOURCES}
)

# --- Includes ---
target_include_directories(${PROJECT_NAME} PRIVATE 
    src                   
    ${lvgl_SOURCE_DIR}    
    ${SDL2_INCLUDE_DIRS}
)

# --- Linking ---
target_link_libraries(${PROJECT_NAME} PRIVATE 
    ${SDL2_LIBRARIES}
)

if(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE m pthread)
endif()
