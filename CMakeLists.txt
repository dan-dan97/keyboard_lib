cmake_minimum_required(VERSION 2.8)
project(keyboard_lib)

find_package(Boost COMPONENTS date_time thread REQUIRED)

include_directories(
    include
    ${Boost_INCLUDE_DIR}
)

set(KEYBOARD_LIB_HEADERS include/keyboard_lib.hpp)
set(KEYBOARD_LIB_SOURCES src/keyboard_lib.cpp)

add_library(keyboard_lib SHARED ${KEYBOARD_LIB_HEADERS} ${KEYBOARD_LIB_SOURCES})
target_link_libraries(keyboard_lib pthread ${Boost_LIBRARIES})

add_executable(keyboard_lib_test examples/keyboard_lib_test.cpp)
target_link_libraries(keyboard_lib_test keyboard_lib)

install(TARGETS keyboard_lib DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES ${KEYBOARD_LIB_HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
install(TARGETS keyboard_lib_test DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
