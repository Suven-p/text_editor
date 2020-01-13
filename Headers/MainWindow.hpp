#pragma once

#include "BaseWindow.hpp"
#include "TabControl.hpp"

namespace Os
{
class MainWindow : public Os::BaseWindow<MainWindow>
{
  private:
    HWND m_child_hwnd;
    TabControl tbctrl;

  public:
    PCSTR ClassName() const
    {
        return "Main Window Class";
    }
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};
} // namespace Os