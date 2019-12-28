set(class_files File.cpp
    Basewindow.cpp
    Mainwindow.cpp
    TextWindow.cpp
    AutoComplete.cpp
    )
set(other_files main.cpp)

#generate header file names from class file names
set(class_headers "")
foreach(STR ${class_files})
    string(REPLACE ".cpp" ".hpp" class_header ${STR})
    list(APPEND class_headers "${CMAKE_SOURCE_DIR}/Headers/${class_header}")
endforeach(STR ${class_files})

set(other_headers "")

set(src ${class_files} ${other_files})

set(headers ${class_headers} ${other_headers})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Executable)

add_executable(${CMAKE_PROJECT_NAME} ${src} ${headers})

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/Headers)

