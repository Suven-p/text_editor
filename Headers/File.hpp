#pragma once
#include <fstream>
#include <windows.h>

class File
{
    private:
    HWND m_text_window;
    std::fstream m_file;


    public:
        void open_file();
};