cmake_minimum_required(VERSION 3.22) 

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(ggj2023) 

include_directories(
    include
)

file(GLOB SOURCES
    src/*.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})

if(WIN32)
    target_link_libraries(${CMAKE_PROJECT_NAME} lithium WSock32)
else()
    target_link_libraries(${CMAKE_PROJECT_NAME} lithium)
endif()

add_subdirectory(dep/lithium) 

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

install(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/bin)