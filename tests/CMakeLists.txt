set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_FLAGS -Wall)

set(src main.cpp test_memory.cpp test_object.cpp)

add_executable(${PROJECT_NAME}_test ${src})
target_link_libraries(${PROJECT_NAME}_test LINK_PUBLIC ${PROJECT_NAME}_lib)