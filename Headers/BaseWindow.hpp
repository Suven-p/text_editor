#pragma once

#include "Os.hpp"

namespace Os
{
template <class DerivedType>
class BaseWindow
{
  public:
    static LRESULT CALLBACK window_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        DerivedType *p_this = NULL;

        // Set USERDATA to this pointer of derived class. The pointer can then be retrieved from USERDATA and used to
        // call handle_message function
        if (msg == WM_NCCREATE)
        {
            // this pointer is passed from CreateWindowEx() as lParam
            CREATESTRUCT *p_create = reinterpret_cast<CREATESTRUCT *>(lParam);
            p_this = (DerivedType *)p_create->lpCreateParams;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)p_this);
            p_this->m_hwnd = hwnd;
        }
        else
        {
            p_this = (DerivedType *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        }
        if (p_this)
        {
            return p_this->handle_message(msg, wParam, lParam);
        }
        else
        {
            return DefWindowProc(hwnd, msg, wParam, lParam);
        }
    }

    BaseWindow() : m_hwnd(NULL)
    {
    }

    bool create(LPCTSTR window_name, DWORD style, DWORD ex_style = 0, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT,
                int width = CW_USEDEFAULT, int height = CW_USEDEFAULT, HWND parent = 0, HMENU menu = 0
                )
    {
        WNDCLASSEX wc = {0};
        // Register class if no class is registered with name
        if (!GetClassInfoEx(GetModuleHandle(NULL), class_name(), &wc))
        {
            wc.lpfnWndProc = DerivedType::window_proc;
            wc.hInstance = GetModuleHandle(NULL);
            wc.lpszClassName = class_name();
            wc.cbSize = sizeof(WNDCLASSEX);
            RegisterClassEx(&wc);
        }
        m_hwnd = CreateWindowEx(
            ex_style, class_name(), window_name, style, x, y, width, height, parent, menu, GetModuleHandle(NULL), this);

        return (m_hwnd != NULL);
    }

    HWND window() const
    {
        return m_hwnd;
    }

  protected:
    virtual LPCTSTR class_name() const = 0;
    virtual LRESULT handle_message(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
    HWND m_hwnd;
};
} // namespace Os