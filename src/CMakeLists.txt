set(SRC_FILES
        ConfigFileParser.cpp
	reader.cpp
	utils.cpp
        )

# Создание библиотеки из исходных файлов
add_library(ConfigFileParser ${SRC_FILES})

# Добавление директории с заголовочными файлами для поиска
target_include_directories(ConfigFileParser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)