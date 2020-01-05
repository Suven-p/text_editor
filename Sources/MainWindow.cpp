#include "MainWindow.hpp"
#include "resource.hpp"

LRESULT Os::MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE: {
        HICON icon_MainWindow = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_TEXT_EDITOR_MainWindow));
        SendMessage(m_hwnd, WM_SETICON, ICON_BIG, (LPARAM)icon_MainWindow);
        SendMessage(m_hwnd, WM_SETICON, ICON_SMALL, (LPARAM)icon_MainWindow);
        m_child_hwnd = CreateWindowEx(0,
                                      "EDIT",
                                      "",
                                      WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
                                      0,
                                      0,
                                      0,
                                      0,
                                      m_hwnd,
                                      (HMENU)ID_TEXT_EDITOR_EDIT1,
                                      (HINSTANCE)GetModuleHandle(NULL),
                                      NULL);
        return 0;
    }
    case WM_SETFOCUS: {
        SetFocus(m_child_hwnd);
        return 0;
    }

    case WM_DESTROY: {
        PostQuitMessage(0);
        return 0;
    }
    case WM_SIZE: {
        MoveWindow(m_child_hwnd, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
        return 0;
    }

    default:
        return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
    }
    return TRUE;
}
}