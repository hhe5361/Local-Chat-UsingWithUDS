cmake_minimum_required(VERSION 3.16)

project(local_chat_server C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic)

include_directories(include)

add_library(chatcore STATIC
    src/chat_message.c
    src/client.c
    src/publisher.c
)

target_include_directories(chatcore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

#thread 관련 라이브러리 
find_package(Threads REQUIRED)

target_link_libraries(chatcore PUBLIC
    Threads::Threads
)

add_executable(server
    src/main.c
)

target_link_libraries(server PRIVATE
    chatcore
)
