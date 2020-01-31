#include "MainWindow.hpp"
#include "Scintilla.h"
#include "TabControl.hpp"
#include "resource.hpp"
#include <vector>

LRESULT Os::MainWindow::handle_message(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE: {
        //INITCOMMONCONTROLSEX icex;
        //icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
        //icex.dwICC = ICC_TAB_CLASSES;
        //InitCommonControlsEx(&icex);

        HICON icon_MainWindow = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_TEXT_EDITOR_MainWindow));
        HMENU main_menu = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDM_TEXT_EDITOR_MainWindow));
        SendMessage(m_hwnd, WM_SETICON, ICON_BIG, (LPARAM)icon_MainWindow);
        SendMessage(m_hwnd, WM_SETICON, ICON_SMALL, (LPARAM)icon_MainWindow);
        SetMenu(m_hwnd, main_menu);
        tbctrl = TabControl(m_hwnd);
        tbctrl.create();
        tbctrl.new_tab(TEXT("Test"));
        tbctrl.new_tab(TEXT("test2"));
        tbctrl.new_tab(TEXT("test3"));
        return 0;
    }

    case WM_DRAWITEM: {
        LPDRAWITEMSTRUCT di = (LPDRAWITEMSTRUCT)lParam;
        HDC hdc = di->hDC;
        RECT rect = di->rcItem;
        if (di->CtlType == ODT_TAB && di->hwndItem == tbctrl.window())
        {
            int tab = di->itemID;
            int st = ::SendMessage(tbctrl.window(), TCM_GETCURSEL, 0, 0);
            bool is_selected = (tab == st);
            TCHAR label[MAX_PATH];
            TCITEM tci;
            tci.mask = TCIF_TEXT | TCIF_IMAGE;
            tci.pszText = label;
            tci.cchTextMax = MAX_PATH - 1;

            SendMessage(tbctrl.window(), TCM_GETITEM, tab, reinterpret_cast<LPARAM>(&tci));
          
            int saved_dc = SaveDC(hdc);

            SetBkMode(hdc, TRANSPARENT);
            HBRUSH hBrush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
            FillRect(hdc, &rect, hBrush);
            ::DeleteObject((HGDIOBJ)hBrush);

            // equalize drawing areas of active and inactive tabs
            int X_padding = 0;
            int Y_padding = 0;
            //if (is_selected)
            {
                // the drawing area of the active tab extends on all borders by default
                rect.top += GetSystemMetrics(SM_CYEDGE);
                rect.bottom -= GetSystemMetrics(SM_CYEDGE);
                rect.left += GetSystemMetrics(SM_CXEDGE);
                rect.right -= GetSystemMetrics(SM_CXEDGE);
                rect.top += X_padding;
                rect.bottom -= Y_padding;
            }
            //else
 /*           {
                rect.left -= X_padding;
                rect.right += X_padding;
                rect.top += Y_padding;
                rect.bottom += Y_padding;
            }*/

             if (is_selected)
            {
                 hBrush = ::CreateSolidBrush(RGB(211, 211, 211)); // #FAAA3C
                FillRect(hdc, &rect, hBrush);
                DeleteObject((HGDIOBJ)hBrush);
            }
             else
            {
                hBrush = ::CreateSolidBrush(RGB(105,105,105));
                FillRect(hdc, &rect, hBrush);
                DeleteObject((HGDIOBJ)hBrush);
            }
            
            TextOutA(hdc, 0, 0, label, strlen(label));
            RestoreDC(hdc, saved_dc);
        }

            
        
    }
                  
    case WM_NOTIFY: 
    {
        LPNMHDR info = (LPNMHDR)lParam;
        switch (info->code)
        {
        case TCN_SELCHANGING: {
            int selected = info->idFrom;
            selected = TabCtrl_GetCurSel(tbctrl.window());
            ShowWindow(tbctrl.tab(selected), SW_HIDE);

            return FALSE;
        }
        case TCN_SELCHANGE: 
        {
            int selected = TabCtrl_GetCurSel(tbctrl.window());
            InvalidateRect(tbctrl.window(), NULL, TRUE);
            UpdateWindow(tbctrl.window());
            ShowWindow(tbctrl.tab(selected), SW_SHOW);
            SetFocus(tbctrl.tab(selected));
            return TRUE;
        }
        default: 
        {
            return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
        }
        }
    }
    case WM_DESTROY: 
    {
        PostQuitMessage(0);
        return 0;
    }
    case WM_SIZE: 
    {
        MoveWindow(tbctrl.window(), 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
        RECT size = tbctrl.get_size();
        TabCtrl_AdjustRect(tbctrl.window(), FALSE, &size);
        for (int i = 0; i < tbctrl.num_tabs(); i++)
        {
            MoveWindow(tbctrl.tab(i), 0, 25, size.right, size.bottom - 25, TRUE);
            int selected = TabCtrl_GetCurSel(tbctrl.window());
            InvalidateRect(tbctrl.tab(selected), NULL, TRUE);
            UpdateWindow(tbctrl.tab(selected));
            ShowWindow(tbctrl.tab(selected), SW_SHOW);
            SetFocus(tbctrl.tab(selected));
        }

        return 0;
    }

    default:
        return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
    }
    return TRUE;
}

LPCTSTR Os::MainWindow::class_name() const
{
    return TEXT("Main Window Class");
}
