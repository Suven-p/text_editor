#pragma once
#include "Os.hpp"
#include <fstream>

class File
{
  private:
    HWND m_text_window;
    std::fstream m_file;

  public:
    File() = delete;
    File(HWND parent) : m_text_window(parent)
    {
    }
    void open_file();
};