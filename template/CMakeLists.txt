CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

add_subdirectory(SDL2)

# change this to change the name of the project
SET(PROJECT_NAME template)

# finds all source files to build
FILE(GLOB SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

# creates a project that builds an executable and uses SDL2
PROJECT(${PROJECT_NAME})
ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCE_FILES})
INCLUDE_DIRECTORIES(${SDL2_SOURCE_DIR}/include)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} SDL2)
