#pragma once

#include "BaseWindow.hpp"
#include "EditControl.hpp"
#include "Os.hpp"
#include <CommCtrl.h>
#include <vector>

#define MAX_TABS 100

namespace Os
{
class TabControl
{
  private:
    HWND m_parent_hwnd;
    HWND m_hwnd;
    std::vector<HWND> m_edit_hwnd;
    RECT parent_rect;
    unsigned int m_num_tabs;

  public:
    TabControl() : m_parent_hwnd(NULL), m_hwnd(NULL), m_num_tabs(0), m_edit_hwnd(MAX_TABS, nullptr)
    {}
    TabControl(HWND parent_hwnd)
        : m_parent_hwnd(parent_hwnd), m_hwnd(NULL), m_num_tabs(0), m_edit_hwnd(MAX_TABS, nullptr)
    {
        GetClientRect(m_parent_hwnd, &parent_rect);

    }
    ~TabControl() = default;
    

    bool create();
    bool new_tab(TCHAR *label);
    bool new_edit(HWND parent);
    HWND window()
    {
        return m_hwnd;
    }
    int num_tabs()
    {
        return m_num_tabs;
    }
    HWND tab(int i)
    {
        return m_edit_hwnd[i];
    }
    RECT get_size()
    {
        RECT rc;
        GetClientRect(m_hwnd, &rc);
        return rc;
    }
};
} // namespace Os