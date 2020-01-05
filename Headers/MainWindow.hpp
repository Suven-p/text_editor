#pragma once

#include "BaseWindow.hpp"
#define ID_TEXT_EDITOR_EDIT1 1000

namespace Os
{
class MainWindow : public Os::BaseWindow<MainWindow>
{
  private:
    HWND m_child_hwnd;

  public:
    PCSTR ClassName() const
    {
        return "Main Window Class";
    }
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};
} // namespace Os