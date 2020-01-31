#pragma once

#include "BaseWindow.hpp"
#include "Os.hpp"
#include "Scintilla.h"

#define ID_TEXT_EDITOR_EDIT 1000

namespace Os
{
class EditControl
{
  private:
    HWND m_hwnd;
    HWND m_parent;
    int (*direct_function)(void *, int, int, int);
    void *direct_pointer;

  public:
    EditControl() = delete;
    EditControl(HWND parent) : m_parent(parent), m_hwnd(NULL), direct_function(NULL), direct_pointer(NULL)
    {
    }
    bool create(int i);
    HWND window() const;
    LPCTSTR class_name() const;
    int SendMsg(int msg, int param1, int param2);
  
};
} // namespace Os
