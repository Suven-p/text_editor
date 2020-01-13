#include "MainWindow.hpp"
#include "TabControl.hpp"
#include "resource.hpp"

LRESULT Os::MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE: {
        HICON icon_MainWindow = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_TEXT_EDITOR_MainWindow));
        SendMessage(m_hwnd, WM_SETICON, ICON_BIG, (LPARAM)icon_MainWindow);
        SendMessage(m_hwnd, WM_SETICON, ICON_SMALL, (LPARAM)icon_MainWindow);
        tbctrl = TabControl(m_hwnd);
        tbctrl.Create();
        tbctrl.New_tab("Test");
        tbctrl.New_tab("test2");
        tbctrl.New_tab("test3");
        return 0;
    }
        
    case WM_NOTIFY: {
        LPNMHDR info = (LPNMHDR)lParam;
        switch (info->code)
        {
            //NM_CHAR
        case TCN_SELCHANGING: {
            int selected = info->idFrom;
            selected = TabCtrl_GetCurSel(tbctrl.Window());
            ShowWindow(tbctrl.Tab(selected), SW_HIDE);

            return FALSE;
        }
        case TCN_SELCHANGE: {
            int selected = TabCtrl_GetCurSel(tbctrl.Window());

            RECT test;
            TabCtrl_GetItemRect(tbctrl.Window(), selected, &test);

            InvalidateRect(tbctrl.Tab(selected), NULL, TRUE);
            UpdateWindow(tbctrl.Tab(selected));
            ShowWindow(tbctrl.Tab(selected), SW_SHOW);
            return TRUE;
        }
        default: {
            
            return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
        }
        }
        
    
    }
    case WM_DESTROY: {
        PostQuitMessage(0);
        return 0;
    }
        case WM_SIZE: {
            MoveWindow(tbctrl.Window(), 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
            return 0;
        }

    default:
        return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
    }
    return TRUE;
}