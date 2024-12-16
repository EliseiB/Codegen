cmake_minimum_required(VERSION 3.16)
project(codegen)

set(CMAKE_CXX_STANDARD 17)

# Добавьте путь к библиотеке JSON
include_directories(${CMAKE_SOURCE_DIR}/json/include)

# Укажите путь к исходному файлу
add_executable(Codegen codegen/codegen.cpp)

find_package(CURL REQUIRED)
target_link_libraries(Codegen CURL::libcurl)