cmake_minimum_required(VERSION 3.20.0)

set(CONF_FILE "prj.conf")
# if(DEBUG)
#list(APPEND CONF_FILE "fragment_file2.conf")
#else()
#endif()
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(app)

include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)

set(CMAKE_CXX_FLAGS "-std=c++17")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -ggdb")
set(CMAKE_CXX_FLAGS_DEBUG "-Og -ggdb")

#set(CMAKE_BUILD_TYPE "RelWithDebInfo")
file(GLOB SRC src/main3.cpp)

target_sources(${PROJECT_NAME} PRIVATE ${SRC})
target_include_directories(app PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

#cmake_minimum_required(VERSION 3.11)
#project(test)
#
#find_package(Zephyr REQUIRED)
#
##set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpic -O0 -ggdb")
#file(GLOB SRC src/main3.cpp)
#target_sources(app PRIVATE
#    ${SRC}
#    )
##add_executable(${PROJECT_NAME} ${SRC})
#target_include_directories(app PUBLIC
#    ${CMAKE_SOURCE_DIR}/include
#    )
#add_subdirectory(test)
