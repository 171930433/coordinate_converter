# vcpkg
file(GLOB TEST_FILES *.cpp)

find_package(GTest CONFIG REQUIRED)

add_executable(main_test ${TEST_FILES})

target_link_libraries(
  main_test  
  PRIVATE ${PROJECT_NAME}::coordinate_converter GTest::gtest GTest::gtest_main)

target_include_directories(main_test PRIVATE ${CMAKE_CURRENT_LIST_DIR})

include(GoogleTest)

gtest_discover_tests(main_test)
