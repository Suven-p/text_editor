#pragma once

#include "BaseWindow.hpp"
#include "EditView.hpp"
#include <stdexcept>
#include <string>

namespace Os
{
class MainWindow : public Os::BaseWindow
{
  public:
    MainWindow() : Os::BaseWindow(::GetModuleHandle(0), 0), wc{0}
    {
    }
    MainWindow(HINSTANCE hinst) : BaseWindow(hinst, 0), wc{0}
    {
    }
    ~MainWindow() = default;
    void create(LPCTSTR window_name, DWORD style, DWORD ex_style = 0, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT,
                int width = CW_USEDEFAULT, int height = CW_USEDEFAULT, HWND parent = 0, HMENU menu = 0);
    static bool dark_mode()
    {
        return use_dark_mode;
    }
    void destroy();
    
    static LRESULT CALLBACK window_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        Os::MainWindow* p_this = NULL;
        // Set USERDATA to this pointer of derived class. The pointer can then be retrieved from USERDATA and used to
        // call handle_message function
        if (msg == WM_NCCREATE)
        {
            // this pointer is passed from CreateWindowEx() as lParam
            CREATESTRUCT* p_create = reinterpret_cast<CREATESTRUCT*>(lParam);
            p_this = (Os::MainWindow*)(p_create->lpCreateParams);
            p_this->m_hwnd = hwnd;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)p_this);
        }
        else
        {
            p_this = (Os::MainWindow*)(GetWindowLongPtr(hwnd, GWLP_USERDATA));
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

  private:
    WNDCLASSEXA wc;
    EditView dc1;
    // TODO: Add subview
    EditControl sce1;
    LPCTSTR class_name() const;
    LRESULT handle_message(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void notify(SCNotification*);
};
} // namespace Os