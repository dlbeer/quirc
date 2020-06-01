cmake_minimum_required(VERSION 3.6.0 FATAL_ERROR)

project(quirc VERSION 0.1 LANGUAGES C)

FILE(GLOB QUIRC_SRC_FILES lib/*.c)

add_library(quirc STATIC ${QUIRC_SRC_FILES})

install(TARGETS quirc EXPORT quirc DESTINATION "lib")
install(FILES "lib/quirc.h" DESTINATION "include/quirc")
install(EXPORT quirc DESTINATION "lib/cmake/quirc")
install(FILES "cmake/quirc-config.cmake" DESTINATION "lib/cmake/quirc")
