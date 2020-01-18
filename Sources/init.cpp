#include <string>
#include "Os.hpp"
#include "Scintilla.h"
#include "parameters.hpp"


HMODULE extern_SCINTILLA = NULL;
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
    extern_SCINTILLA = LoadLibrary(sci_dll.c_str());
    if (extern_SCINTILLA == NULL)
    {
        if (GetLastError() == ERROR_BAD_EXE_FORMAT)
        {
            sci_dll = extern_EXECUTABLE_DIRECTORY + TEXT("\\sci32\\SciLexer.dll");
            extern_SCINTILLA = LoadLibrary(sci_dll.c_str());
        }
        if (extern_SCINTILLA == NULL)
        {
            char error[1024];
            sprintf(error, "Error %d: Cannot Load Scantilla DLL", GetLastError());
            MessageBoxA(NULL, error, "Error!", MB_OK | MB_ICONERROR);
            return false;
        }
    }
    return true;
}