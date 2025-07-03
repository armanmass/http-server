find_package(GTest REQUIRED)

add_executable(run_tests test_parser.cpp)

target_link_libraries(run_tests PRIVATE GTest::GTest GTest::Main)

target_include_directories(run_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)