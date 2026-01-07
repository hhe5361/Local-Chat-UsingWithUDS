cmake_minimum_required(VERSION 3.16)

project(local_chat_client C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic)

include_directories(include)

find_package(Threads REQUIRED)
find_package(Curses REQUIRED)

# reference link
#보통 소스 아래에 주르르 나열
add_executable(client
    src/main.c
    src/uiRenderer.c
    src/input_loop.c
)

target_link_libraries(client
    Threads::Threads
    ${CURSES_LIBRARIES}
)

target_include_directories(client PRIVATE
    ${CURSES_INCLUDE_DIRS}
)
