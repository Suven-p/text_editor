#include "Os.hpp"
#include <string>
#include <windows.h>

using namespace std;

wstring Os::get_file_name(HWND owner)
{
    OPENFILENAMEW ofn = {};         // common dialog box structure
    WCHAR file_name[MAX_PATH] = L""; // buffer for file name
    HANDLE hf;                     // file handle

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = owner;
    ofn.lpstrFile = file_name;
    ofn.nMaxFile = sizeof(file_name) / sizeof(file_name[0]);
    ofn.lpstrFilter = L"All\0*.*\0Text\0*.TXT\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // Display the Open dialog box.

    if (GetOpenFileNameW(&ofn) == TRUE)
        return static_cast<wstring>(file_name);
    return L"";
}

void Os::get_current_dir(LPTSTR buffer, int size)
{
    GetCurrentDirectory(size, buffer);
}