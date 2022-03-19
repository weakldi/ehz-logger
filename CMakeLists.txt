
cmake_minimum_required(VERSION 3.10)

# set the project name
project(ehz-logger)

add_custom_target(libsml 
    COMMAND make all
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/third-party/libsml
)

set(src_files 
    main.c
)

message(${CMAKE_CURRENT_SOURCE_DIR}/third-party/libsml/sml/include)
add_executable(ehz-logger ${src_files})
add_executable(ehz-logger-test01 test.c)

target_link_libraries(ehz-logger ${CMAKE_CURRENT_SOURCE_DIR}/third-party/libsml/sml/lib/libsml.a)
target_link_libraries(ehz-logger uuid)
target_include_directories(ehz-logger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/third-party/libsml/sml/include)

target_link_libraries(ehz-logger-test01  ${CMAKE_CURRENT_SOURCE_DIR}/third-party/libsml/sml/lib/libsml.a)
target_link_libraries(ehz-logger-test01  uuid)
target_include_directories(ehz-logger-test01  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/third-party/libsml/sml/include)