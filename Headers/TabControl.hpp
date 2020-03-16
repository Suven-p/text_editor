#pragma once

#define UNICODE
#ifndef _WIN32_IE
#define _WIN32_IE 0x0600
#endif

#include "BaseWindow.hpp"
#include <commctrl.h>
#include <string>

#define TCN_TABDROPPED        (WM_USER + 1)
#define TCN_TABDROPPEDOUTSIDE (WM_USER + 2)

namespace
{
constexpr int margin = 8;
}

class TabControl : public Os::BaseWindow
{
  public:
    TabControl()
        : BaseWindow(), m_num_tabs(0), m_has_img(false), m_hfont(NULL), m_drag_point(POINT{0, 0}), m_source_tab(0),
          m_tab_dragged(0), m_default_proc(NULL), m_is_dragged(false), m_is_dragging_inside(false){};
    virtual ~TabControl()
    {
        if (m_hwnd)
            destroy();
    };
    virtual void destroy();
    int num_tabs() const
    {
        return m_num_tabs;
    };
    void init(HINSTANCE hInst, HWND hwnd);
    int insert(const std::wstring &name);
    void activate(int index);
    virtual void resize_to(RECT &rc);
    void delete_item(int index);
    void delete_all();
    void set_image(HIMAGELIST himg);
    static void toggle_dragndrop(bool dragndrop)
    {
        m_enable_dragdrop = dragndrop;
    };
    static bool is_dragndrop_enabled()
    {
        return m_enable_dragdrop;
    };
    int get_src_tab_index() const
    {
        return m_source_tab;
    };
    int get_dragged_tab_index() const
    {
        return m_tab_dragged;
    };
    POINT get_drag_point() const
    {
        return m_drag_point;
    };

  protected:
    unsigned char m_num_tabs;
    bool m_has_img;
    HFONT m_hfont;
    WNDPROC m_default_proc;
    static bool m_enable_dragdrop;
    bool m_is_dragged;
    bool m_is_dragging_inside;
    int m_source_tab;
    int m_tab_dragged;
    POINT m_drag_point;
    void exchangeItemData(POINT point);

    void draw_item(DRAWITEMSTRUCT* dis);
    void dragging_cursor(POINT point);
    static LRESULT CALLBACK tabbar_proc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
    {
        return ((TabControl *)(::GetWindowLongPtr(hwnd, GWLP_USERDATA)))->handle_message(hwnd, Message, wParam, lParam);
    };
    LRESULT handle_message(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
};