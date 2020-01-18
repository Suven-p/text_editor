#pragma once
#include "Os.hpp"
#include <string>

#ifdef UNICODE
#define UNICODE_EXE_DIR
extern std::wstring extern_EXECUTABLE_DIRECTORY;
#else
extern std::string extern_EXECUTABLE_DIRECTORY;
#endif

extern HMODULE extern_scintilla;