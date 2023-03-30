add_executable(ConfigParser main.cpp)

target_link_libraries(ConfigParser parser)
target_include_directories(ConfigParser PRIVATE ${PROJECT_SOURCE_DIR})
