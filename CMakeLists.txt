cmake_minimum_required(VERSION 3.28)

project(BasicC2)

add_executable(bc2)
target_sources(bc2 PRIVATE src/main.cpp)

add_library(Bot STATIC)
target_sources(Bot
    PRIVATE src/bot.cpp
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS include
    FILES include/bot.hpp
)

target_link_libraries(bc2 PRIVATE Bot)