cmake_minimum_required(VERSION 3.10)
project(CodegenProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Создание статической библиотеки из codegen.cpp
add_library(codegen STATIC
    codegen/codegen.cpp
)

# Подключение библиотек nlohmann_json и CURL
find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)

target_link_libraries(codegen PRIVATE CURL::libcurl nlohmann_json::nlohmann_json)

# Укажите путь к заголовкам (если есть .h файлы)
target_include_directories(codegen PUBLIC ${CMAKE_SOURCE_DIR}/codegen)