cmake_minimum_required(VERSION 3.14)
project(CCompiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(compiler main.cpp)

target_include_directories(compiler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  # Specify the commit you depend on and update it regularly.
  URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(tests tests.cpp)
target_link_libraries(tests gtest_main)
target_include_directories(tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

include(GoogleTest)
gtest_discover_tests(tests)

