cmake_minimum_required(VERSION 3.12)

project(
        ConfigFileParser
        VERSION 1.0
        DESCRIPTION "Config file parser"
        LANGUAGES CXX
)



set(CMAKE_CXX_STANDARD 17)


add_subdirectory(src)
add_subdirectory(example)
enable_testing()
add_subdirectory(tests)