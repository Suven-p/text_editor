#pragma once

//#define NTDDI_VERSION 0x06010000
//#define _WIN32_WINNT  NTDDI_VERSION
//#define STRICT
#include "Os.hpp"

namespace Os
{
class BaseWindow
{
  public:
    BaseWindow() : m_hwnd(NULL), m_hparent(NULL), m_hInstance(NULL){};
    BaseWindow(HINSTANCE hInst, HWND parent) : m_hInstance(hInst), m_hparent(parent), m_hwnd(NULL){};
    virtual ~BaseWindow() = default;

    virtual void init(HINSTANCE hInst, HWND parent)
    {
        m_hInstance = hInst;
        m_hparent = parent;
    }

    virtual void destroy() = 0;

    virtual void display(bool show = true) const
    {
        ::ShowWindow(m_hwnd, (show) ? SW_SHOW : SW_HIDE);
    };
    virtual void resize_to(RECT &rc)
    {
        ::MoveWindow(m_hwnd, rc.left, rc.top, rc.right, rc.bottom, TRUE);
        update();
    };
    virtual void update() const
    {
        ::InvalidateRect(m_hwnd, NULL, TRUE);
        ::UpdateWindow(m_hwnd);
    };
    virtual void get_rect(RECT &rc) const
    {
        ::GetClientRect(m_hwnd, &rc);
    };
    virtual int get_width() const
    {
        RECT rc;
        get_rect(rc);
        return rc.right - rc.left;
    }
    virtual int get_height() const
    {
        RECT rc;
        get_rect(rc);
        return rc.bottom - rc.top;
    }
    HWND window() const
    {
        return m_hwnd;
    };
    void get_focus() const
    {
        ::SetFocus(m_hwnd);
    };
    HINSTANCE get_instance() const
    {
        return m_hInstance;
    };

  protected:
    HINSTANCE m_hInstance;
    HWND m_hparent;
    HWND m_hwnd;
    static bool use_dark_mode;
};
} // namespace Os