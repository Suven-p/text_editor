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
    LPCTSTR class_name() const;
    LRESULT handle_message(UINT uMsg, WPARAM wParam, LPARAM lParam);
};
} // namespace Os