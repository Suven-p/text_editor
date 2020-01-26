#pragma once
#include "Os.hpp"
#include "SciLexer.h"
#include "Scintilla.h"
#include <string>

namespace Settings
{
#ifdef UNICODE
#define UNICODE_EXE_DIR
extern std::wstring extern_EXECUTABLE_DIRECTORY;
#else
extern std::string extern_EXECUTABLE_DIRECTORY;
#endif
extern HMODULE extern_scintilla;

extern BOOL USE_DARK_BK;

namespace Styles
{
    extern COLORREF externc_back_color, externc_fore_default, externc_cursor, externc_line_number;
}
} // namespace Settings