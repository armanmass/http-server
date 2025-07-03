cmake_minimum_required(VERSION 3.22)

project(HTTPServer VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost 1.74.0 REQUIRED COMPONENTS system)
find_package(Threads REQUIRED)

add_executable(http_server 
    src/main.cpp
    src/http_response.cpp
    src/request_handler.cpp
)

#target_sources(http_server PRIVATE src/http_response.cpp src/request_handler.cpp)

target_link_libraries(http_server PRIVATE Boost::system)

target_include_directories(http_server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(http_server PRIVATE ${Boost_INCLUDE_DIRS})

if(UNIX)
    target_link_libraries(http_server PRIVATE Threads::Threads)
endif()

enable_testing()
add_subdirectory(tests)