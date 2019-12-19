set(src file.cpp
    init.cpp
    main.cpp
    system.cpp
    trie.cpp)

set(headers ${CMAKE_SOURCE_DIR}/Headers/fatal_error.hpp
    ${CMAKE_SOURCE_DIR}/Headers/file.hpp
    ${CMAKE_SOURCE_DIR}/Headers/init.hpp
    ${CMAKE_SOURCE_DIR}/Headers/system.hpp)

include_directories(${CMAKE_SOURCE_DIR}/Headers)

add_executable(${CMAKE_PROJECT_NAME} ${src})
