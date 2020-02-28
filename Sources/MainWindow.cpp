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
    // TODO: Create (bottombar?)
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
            ::SetFocus(sce1.window());
        }
        catch (std::runtime_error re)
        {
            MessageBox(0, re.what(), "Error!", MB_OK | MB_ICONERROR);
        }
        return true;
    }
    case WM_DRAWITEM: {
        DRAWITEMSTRUCT* dis = (DRAWITEMSTRUCT*)lParam;
        if (dis->CtlType == ODT_TAB)
        {
            return ::SendMessage(dis->hwndItem, WM_DRAWITEM, wParam, lParam);
        }
    }
    case WM_NOTIFY: {
        notify(reinterpret_cast<SCNotification*>(lParam));
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

void Os::MainWindow::create(
    LPCTSTR window_name, DWORD style, DWORD ex_style, int x, int y, int width, int height, HWND parent, HMENU menu)
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
    HWND m_hwnd = CreateWindowEx(ex_style,
                                 class_name(),
                                 window_name,
                                 style,
                                 x,
                                 y,
                                 width,
                                 height,
                                 parent,
                                 menu,
                                 m_hInstance,
                                 this);
    if (!m_hwnd)
    {
        auto error_code = GetLastError();
        std::string err_msg = "Error " + std::to_string(error_code) + " Cannot create window of class " + class_name();
        throw std::runtime_error(err_msg);
    }
}

void Os::MainWindow::notify(SCNotification* notification)
{
    switch (notification->nmhdr.code)
    {
    case TCN_TABDROPPED: {
        TabControl* sender = reinterpret_cast<TabControl*>(notification->nmhdr.idFrom);
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
    case SCN_CHARADDED: {
        int pos = sce1.execute(SCI_GETCURRENTPOS, 0, 0);
        switch (static_cast<char>(notification->ch))
        {
        case '<': {
            int word_end = pos;
            while (word_end) {
                char c = sce1.execute(SCI_GETCHARAT, word_end);
                if (isspace(c))
                    word_end--;
            }
            int word_start = sce1.execute(SCI_WORDSTARTPOSITION, pos, true);
            MessageBox(0, std::to_string(word_start).c_str(), std::to_string(pos).c_str(), 0);
            char to_insert[] = ">";
            sce1.execute(SCI_ADDTEXT, strlen(to_insert), (LPARAM)to_insert);
            sce1.execute(SCI_GOTOPOS, pos);
            break;
        }
        case '[': {
            char to_insert[] = "]";
            sce1.execute(SCI_ADDTEXT, strlen(to_insert), (LPARAM)to_insert);
            sce1.execute(SCI_GOTOPOS, pos);
            break;
        }
        // TODO: Handle multiline function arguments
        case '(': {
            char to_insert[] = ")";
            sce1.execute(SCI_ADDTEXT, strlen(to_insert), (LPARAM)to_insert);
            sce1.execute(SCI_GOTOPOS, pos);
            break;
        }
        case '{': {
            // TODO: Handle logic in \n
            intptr_t current_line = sce1.execute(SCI_LINEFROMPOSITION, pos);
            // 0x400 is base level folding
            int fold_level = (sce1.execute(SCI_GETFOLDLEVEL, current_line) & SC_FOLDLEVELNUMBERMASK) - 0x400;
            int bracket_indent = fold_level * sce1.get_indent_len();
            sce1.execute(SCI_SETLINEINDENTATION, current_line, bracket_indent);
            std::string to_insert = "\n";
            sce1.execute(SCI_ADDTEXT, to_insert.size(), (LPARAM)to_insert.c_str());
            // line after { requires extra level of indenting
            int other_indent = (fold_level + 1) * sce1.get_indent_len();
            // TODO: Add support for tab based indenting
            to_insert = std::string(other_indent, ' ');
            sce1.execute(SCI_ADDTEXT, to_insert.size(), (LPARAM)to_insert.c_str());
            // Store current position to restore later
            pos = sce1.execute(SCI_GETCURRENTPOS, 0, 0);
            to_insert = "\n}";
            sce1.execute(SCI_ADDTEXT, to_insert.size(), (LPARAM)to_insert.c_str());
            sce1.execute(SCI_SETLINEINDENTATION, current_line + 2, bracket_indent);
            sce1.execute(SCI_GOTOPOS, pos);
            break;
        }
        // TODO: Handle brace closes
        case '\n': {
            // int current_line = sce1.execute(SCI_LINEFROMPOSITION, pos);
            // int fold_level = sce1.execute(SCI_GETFOLDLEVEL, current_line);
            // sce1.execute(SCI_SETINDENT, sce1.get_indent_len() * fold_level);
        }
        default:
            break;
        }

        break;
    }
    case SCN_MODIFIED: {
        if ((notification->modificationType & SC_PERFORMED_USER) &&
            (notification->modificationType & SC_MOD_INSERTTEXT))
        {
            // TODO: Format document on paste
            /*std::string msg = "Add at " + std::to_string(notification->position) + " of length " +
                              std::to_string(notification->length);
            msg += " Now at " + std::to_string(sce1.execute(SCI_GETCURRENTPOS));
            MessageBox(0, msg.c_str(), "add", 0);*/
        }
        break;
    }
    case SCN_UPDATEUI: {
        // MessageBox(0, "a", "a", 0); if (notification->updated == SC_UPDATE_SELECTION)
        {
            static int brace1 = 0, brace2 = 0;
            int pos = sce1.execute(SCI_GETCURRENTPOS, 0, 0);
            char ch = static_cast<char>(sce1.execute(SCI_GETCHARAT, pos));
            if (ch == '(' || ch == '{' || ch == '[' || ch == '<')
            {
                int other_brace = sce1.execute(SCI_BRACEMATCH, pos);
                if (other_brace != -1 && (other_brace - pos) > 1)
                {
                    sce1.execute(SCI_BRACEHIGHLIGHT, pos, other_brace);
                }
            }
            else if (ch == ')' || ch == '}' || ch == ']' || ch == '>')
            {
                int other_brace = sce1.execute(SCI_BRACEMATCH, pos);
                if (other_brace != -1 && (pos - other_brace) > 1)
                {
                    sce1.execute(SCI_BRACEHIGHLIGHT, other_brace, pos);
                }
            }
            else
            {
                sce1.execute(SCI_BRACEHIGHLIGHT, -1, -1);
            }
        }
        break;
    }

    default:
        break;
    } // End of switch
}