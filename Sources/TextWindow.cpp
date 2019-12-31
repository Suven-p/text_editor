#include "TextWindow.hpp"

LRESULT Os::TextWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_SETFOCUS:
            SetFocus(m_hwnd);
            return 0;

        case WM_SIZE:
        {
            RECT text_window_pos = {};
            GetWindowRect(GetWindow(m_hwnd, GW_OWNER), &text_window_pos);
            MoveWindow(m_hwnd,
                       0, 20,                  // starting x- and y-coordinates
                       (text_window_pos.right - text_window_pos.left) -20, text_window_pos.bottom - text_window_pos.top - 60,
                       //LOWORD(lParam),        // width of client area
                       //HIWORD(lParam),        // height of client area
                       TRUE);                 // repaint window
            return 0;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        default:
            return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
    }
}