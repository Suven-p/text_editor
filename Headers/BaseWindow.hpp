#pragma once
#include "Os.hpp"
namespace Os
{
class BaseWindow
{
  public:
    BaseWindow() : m_hwnd(NULL), m_hparent(NULL), m_hInstance(NULL){};
    BaseWindow(HINSTANCE hInst, HWND parent) : m_hInstance(hInst), m_hparent(parent), m_hwnd(NULL){};
    virtual ~BaseWindow() = default;
    virtual void destroy() = 0;
    virtual void init(HINSTANCE hinst, HWND parent) : m_hInstance(hinst), m_hparent(parent){}
    virtual void display(bool show = true) const
    {
        ::ShowWindow(m_hwnd, (show) ? SW_SHOW : SW_HIDE);
    }
    virtual void resize_to(RECT& rc)
    {
        ::MoveWindow(m_hwnd, rc.left, rc.top, rc.right, rc.bottom, TRUE);
        update();
    };
    virtual void update() const
    {
        ::InvalidateRect(m_hwnd, NULL, TRUE);
        ::UpdateWindow(m_hwnd);
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
    HINSTANCE m_hInstance{nullptr};
    HWND m_hparent{nullptr};
    HWND m_hwnd{nullptr};
    static bool use_dark_mode;
};
} // namespace Os