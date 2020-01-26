cmake_minimum_required(VERSION 3.5)

project(CoroExamples LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5Core)

add_library(cppcoro STATIC IMPORTED)
set_property(TARGET cppcoro PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/cppcoro/build/windows_x64_msvc15_debug/lib/cppcoro.lib")
target_include_directories(cppcoro INTERFACE "${PROJECT_SOURCE_DIR}/cppcoro/include")

add_executable(CoroExamples coroutine_handle.h promise_type.h awaiter.h generator.h main.cpp)
target_compile_options(CoroExamples PRIVATE "/AWAIT")
target_link_libraries(CoroExamples cppcoro Qt5::Core)
