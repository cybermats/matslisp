set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS -Wall)

set(src main.c
        memory.c
        object.c
        io.c
        error_handling.c
        strings.c
        functions.c)

add_executable(${PROJECT_NAME} ${src})
add_library(${PROJECT_NAME}_lib ${src})
target_include_directories(${PROJECT_NAME}_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})