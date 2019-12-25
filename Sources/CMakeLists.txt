set(src file.cpp
    AutoComplete.cpp)

set(headers ${CMAKE_SOURCE_DIR}/Headers/AutoComplete.hpp
    ${CMAKE_SOURCE_DIR}/Headers/file.hpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Executable)

add_executable(${CMAKE_PROJECT_NAME} ${src} ${headers})
include_directories(${CMAKE_SOURCE_DIR}/Headers)
