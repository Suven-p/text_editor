#pragma once

#include "BaseWindow.hpp"
#include <CommCtrl.h>

#define ID_TEXT_EDITOR_EDIT 1000
#define MAX_TABS            100

namespace Os
{
class TabControl
{
  private:
    HWND m_parent_hwnd;
    HWND m_hwnd;
    HWND m_edit_hwnd[MAX_TABS];
    RECT parent_rect;
    unsigned int m_num_tabs;

  public:
    TabControl() : m_parent_hwnd(NULL), m_num_tabs(0), m_hwnd(NULL), m_edit_hwnd()
    {
    }

    TabControl(HWND parent_hwnd)
    {
        m_parent_hwnd = parent_hwnd;
        INITCOMMONCONTROLSEX icex;
        icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
        icex.dwICC = ICC_TAB_CLASSES;
        InitCommonControlsEx(&icex);
        m_num_tabs = 0;
        for (int i = 0; i < MAX_TABS; i++)
            m_edit_hwnd[i] = nullptr;
        m_hwnd = 0;
        GetClientRect(m_parent_hwnd, &parent_rect);
    }
    ~TabControl() = default;

    BOOL Create();
    BOOL New_tab(TCHAR *label);
    BOOL New_edit(HWND parent);
    HWND Window()
    {
        return m_hwnd;
    }
    HWND Tab(int i)
    {
        return m_edit_hwnd[i];
    }
    RECT Get_size()
    {
        RECT rc;
        GetClientRect(m_hwnd, &rc);
        return rc;
    }
};
} // namespace Os