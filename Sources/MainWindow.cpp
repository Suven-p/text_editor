#include "MainWindow.hpp"
#include "EditView.hpp"
#include "Scintilla.h"
#include "TabControl.hpp"
#include "resource.hpp"
#include <vector>

LRESULT Os::MainWindow::handle_message(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE: {
        dc1.toggle_dragndrop(true);
        try
        {
            sce1.init(GetModuleHandle(0), m_hwnd);
            dc1.init(GetModuleHandle(0), m_hwnd, &sce1);
            sce1.set_marker_style(FolderStyle::FOLDER_STYLE_BOX);
            dc1.newDoc("D:\\Programming\\nppv1\\SysMsg.h");
            dc1.display();
            dc1.activate(0);
            sce1.display();
        }
        catch (std::runtime_error re)
        {
            MessageBox(0, re.what(), "Error!", MB_OK | MB_ICONERROR);
        }
        return true;
    }
    case WM_DRAWITEM: {
        DRAWITEMSTRUCT *dis = (DRAWITEMSTRUCT *)lParam;
        if (dis->CtlType == ODT_TAB)
        {
            return ::SendMessage(dis->hwndItem, WM_DRAWITEM, wParam, lParam);
        }
    }
    case WM_NOTIFY: {
        notify(reinterpret_cast<SCNotification *>(lParam));
        return FALSE;
    }

    case WM_SIZE: {
        int tab_height = 40;
        MoveWindow(dc1.window(), 0, 0, LOWORD(lParam), tab_height, true);
        MoveWindow(sce1.window(), 0, 25, LOWORD(lParam), HIWORD(lParam), true);
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
    }
    return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}

LPCTSTR Os::MainWindow::class_name() const
{
    return TEXT("Main Window Class");
}

void Os::MainWindow::destroy()
{
    ::DestroyWindow(m_hwnd);
}

void Os::MainWindow::create(LPCTSTR window_name, DWORD style, DWORD ex_style, int x, int y, int width, int height,
                            HWND parent, HMENU menu)
{
    if (!GetClassInfoEx(GetModuleHandle(NULL), class_name(), &wc))
    {
        wc = {};
        wc.lpfnWndProc = window_proc;
        wc.hInstance = m_hInstance;
        wc.lpszClassName = class_name();
        wc.cbSize = sizeof(WNDCLASSEXA);
        if (!RegisterClassEx(&wc))
        {
            auto error_code = GetLastError();
            std::string err_msg = "Error " + std::to_string(error_code) + " Cannot register class for " + class_name();
            throw std::runtime_error(err_msg);
        }
    }
    HWND m_hwnd = CreateWindowEx(
        ex_style, class_name(), window_name, style, x, y, width, height, parent, menu, m_hInstance, this);
    if (!m_hwnd)
    {
        auto error_code = GetLastError();
        std::string err_msg = "Error " + std::to_string(error_code) + " Cannot create window of class " + class_name();
        throw std::runtime_error(err_msg);
    }
}

void Os::MainWindow::notify(SCNotification *notification)
{
    switch (notification->nmhdr.code)
    {
    case TCN_TABDROPPED: {
        TabControl *sender = reinterpret_cast<TabControl *>(notification->nmhdr.idFrom);
        int dest_index = sender->get_dragged_tab_index();
        int src_index = sender->get_src_tab_index();
        sce1.swap_buffer(dest_index, src_index);
        dc1.activate(dest_index);
        break;
    }
    case TCN_SELCHANGE: {
        if (notification->nmhdr.hwndFrom == dc1.window())
        {
            dc1.clickedUpdate();
        }
        break;
    }
    case SCN_MARGINCLICK: {
        if (notification->margin == EditControl::m_folder_margin)
        {
            sce1.margin_click(notification->position, notification->modifiers);
        }
        else if (notification->margin == EditControl::m_symbol_margin)
        {
            sce1.execute(SCI_LINEFROMPOSITION, notification->position);
        }
        break;
        return;
    }
    default:
        break;
    }// End of switch

}