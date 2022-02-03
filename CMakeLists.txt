cmake_minimum_required(VERSION 3.0)

project(cplus LANGUAGES CXX)

find_package(FLEX)
find_package(BISON)

FLEX_TARGET(MyScanner lexer.l  ${CMAKE_BINARY_DIR}/lexer.cpp)
BISON_TARGET(MyParser parser.y ${CMAKE_BINARY_DIR}/parser.cpp)
ADD_FLEX_BISON_DEPENDENCY(MyScanner MyParser)

set(HEADERS "shell.h")

set(SOURCES "main.cpp" "shell.cpp")

add_executable(cplus ${HEADERS} ${SOURCES} ${BISON_MyParser_OUTPUTS} ${FLEX_MyScanner_OUTPUTS})

target_include_directories(cplus PUBLIC ${FLEX_INCLUDE_DIRS} PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})

set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT cplus)