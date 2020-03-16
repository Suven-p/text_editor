#include "EditControl.hpp"
#include "Os.hpp"
#include "Scintilla.h"
#include "parameters.hpp"
#include <string>

#ifdef UNICODE_EXE_DIR
std::wstring extern_EXECUTABLE_DIRECTORY;
std::wstring sci_dll;
#else
std::string extern_EXECUTABLE_DIRECTORY;
#endif

using namespace std;
bool init()
{
    TCHAR current[MAX_PATH];
    Os::get_current_dir(current, MAX_PATH);
    extern_EXECUTABLE_DIRECTORY = current;
    auto sci_dll = extern_EXECUTABLE_DIRECTORY + TEXT("\\sci64\\SciLexer.dll");
    return true;
}