set(class_files 
    AutoComplete.cpp
    EditControl.cpp
    EditView.cpp
    ErrorMessage.cpp
    File.cpp
    Mainwindow.cpp
    Menu.cpp
    Os.cpp
    TabControl.cpp
    Trie.cpp
)
set(other_files
    main.cpp
	init.cpp
)

#generate header file names from class file names
set(class_headers "")
foreach(STR ${class_files})
    string(REPLACE ".cpp" ".hpp" class_header ${STR})
    list(APPEND class_headers "${CMAKE_SOURCE_DIR}/Headers/${class_header}")
endforeach(STR ${class_files})

set(other_headers 
	"${CMAKE_SOURCE_DIR}/Resources/resource.hpp"
	"${CMAKE_SOURCE_DIR}/Headers/parameters.hpp"
	"${CMAKE_SOURCE_DIR}/Headers/BaseWindow.hpp"
	"${CMAKE_SOURCE_DIR}/Headers/Buffer.hpp"
    "${CMAKE_SOURCE_DIR}/Headers/Colors.hpp"
)

set(src 
	${class_files} 
	${other_files}
)
set(headers 
	${class_headers} 
	${other_headers}
)
set(resources 
	#"${CMAKE_SOURCE_DIR}/Resources/text_editor.rc"
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Executable)

add_executable(${CMAKE_PROJECT_NAME} WIN32 ${src} ${headers} ${resources})

target_include_directories(${CMAKE_PROJECT_NAME} 
	PUBLIC "${CMAKE_SOURCE_DIR}/Headers"
	PUBLIC "${CMAKE_SOURCE_DIR}/Resources"
	PUBLIC "${CMAKE_SOURCE_DIR}/scintilla/include"
)
target_link_libraries(${CMAKE_PROJECT_NAME} 
		Shlwapi.lib
		Comctl32.lib
)

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES 
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug"
)