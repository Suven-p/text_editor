#include <string>
#include <windows.h>
#include "Os.hpp"

using namespace std;

string Os::get_file_name(HWND owner)
{
    OPENFILENAME ofn = {};       // common dialog box structure
    char file_name[MAX_PATH] = "";       // buffer for file name
    HANDLE hf;              // file handle

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = owner;
    ofn.lpstrFile = file_name;
    ofn.nMaxFile = sizeof(file_name) / sizeof(file_name[0]);
    ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        // Display the Open dialog box.

    if (GetOpenFileName(&ofn)==TRUE)
        return static_cast<string>(file_name);
    return string("");
}