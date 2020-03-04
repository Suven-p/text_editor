#include "MainWindow.hpp"
#include "EditView.hpp"
#include "Menu.hpp"
#include "Scintilla.h"
#include "TabControl.hpp"
#include "resource.hpp"
#include <vector>

Menu file_menu = Menu("File",
                      {Menu("New", {Menu("Header File (.hpp)"), Menu("Source File (.cpp)"), Menu("Text File (.txt)")}),
                       Menu("Open"),
                       Menu("Save"),
                       Menu("Save As"),
                       Menu("Exit")});

Menu edit_menu = Menu("Edit", {Menu("Copy"), Menu("Cut"), Menu("Paste"), Menu("Find"), Menu("Find and Replace")});

LRESULT Os::MainWindow::handle_message(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    // TODO: Create (bottombar?)
    case WM_CREATE: {
        dc1.toggle_dragndrop(true);
        HMENU hfile_menu = file_menu.init();
        HMENU main_menu = CreateMenu();
        std::vector<Menu> main_menu_arr = {file_menu, edit_menu};
        for (auto var : main_menu_arr)
        {
            HMENU menu = var.init();
            if (!AppendMenu(main_menu, MF_POPUP, (UINT_PTR)menu, var.get_name().c_str()))
            {
                std::string err_msg = "Error " + std::to_string(GetLastError()) + " when appending.";
                throw std::runtime_error(err_msg);
            }
        }
        if (!SetMenu(m_hwnd, main_menu))
        {
            std::string err_msg = "Error" + std::to_string(GetLastError()) + " when setting menu.";
            throw std::runtime_error(err_msg);
        }
        try
        {
            sce1.init(GetModuleHandle(0), m_hwnd);
            dc1.init(GetModuleHandle(0), m_hwnd, &sce1);
            sce1.set_marker_style(FolderStyle::FOLDER_STYLE_BOX);
            dc1.newDoc(L"D:\\Programming\\nppv1\\SysMsg.h");
            dc1.newDoc(L"D:\\Programming\\NPP(v1 src)\\PowerEditor\\src\\WinControls\\TabBar\\TabBar.cpp");
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
    case WM_COMMAND: {
        if (LOWORD(wParam) == 106)
            PostQuitMessage(0);
        break;
    }
    case WM_SIZE: {
        int tab_height = 40;
        MoveWindow(dc1.window(), 0, 0, LOWORD(lParam), tab_height, true);
        MoveWindow(sce1.window(), 0, 25, LOWORD(lParam), HIWORD(lParam), true);
        break;
    }
    case WM_SETFOCUS: {
        sce1.get_focus();
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
            std::string prev = sce1.get_previous_word(pos - 2);
            if (prev == "include" || prev == "template")
            {
                char to_insert[] = ">";
                sce1.execute(SCI_ADDTEXT, strlen(to_insert), (LPARAM)to_insert);
                sce1.execute(SCI_GOTOPOS, pos);
            }
            break;
        }
        case '>': {
            char next = sce1.execute(SCI_GETCHARAT, pos);
            if (next == '>')
            {
                sce1.execute(SCI_DELETERANGE, pos - 1, 1);
                sce1.execute(SCI_GOTOPOS, pos);
            }
            break;
        }
        case '[': {
            char to_insert[] = "]";
            sce1.execute(SCI_ADDTEXT, strlen(to_insert), (LPARAM)to_insert);
            sce1.execute(SCI_GOTOPOS, pos);
            break;
        }
        case ']': {
            char next = sce1.execute(SCI_GETCHARAT, pos);
            if (next == ']')
            {
                sce1.execute(SCI_DELETERANGE, pos - 1, 1);
                sce1.execute(SCI_GOTOPOS, pos);
            }
            break;
        }
        // TODO: Handle multiline function arguments
        case '(': {
            char to_insert[] = ")";
            sce1.execute(SCI_ADDTEXT, strlen(to_insert), (LPARAM)to_insert);
            sce1.execute(SCI_GOTOPOS, pos);
            break;
        }
        case ')': {
            char prev = sce1.execute(SCI_GETCHARAT, pos - 2);
            char next = sce1.execute(SCI_GETCHARAT, pos);
            if (next == ')')
            {
                sce1.execute(SCI_DELETERANGE, pos - 1, 1);
                sce1.execute(SCI_GOTOPOS, pos);
            }
            break;
        }
        case '{': {
            intptr_t current_line = sce1.execute(SCI_LINEFROMPOSITION, pos);
            int fold_level = (sce1.execute(SCI_GETFOLDLEVEL, current_line) & SC_FOLDLEVELNUMBERMASK) - 0x400;
            int bracket_indent = fold_level * sce1.get_indent_len();
            sce1.execute(SCI_SETLINEINDENTATION, current_line, bracket_indent);
            std::string to_insert = "}";
            sce1.execute(SCI_ADDTEXT, to_insert.size(), (LPARAM)to_insert.c_str());
            pos = sce1.execute(SCI_GETCURRENTPOS, 0, 0);
            // sce1.execute(SCI_SETLINEINDENTATION, current_line + 1, bracket_indent);
            sce1.execute(SCI_GOTOPOS, pos - 1);
            break;
        }
        // TODO: Handle brace closes
        case '\n': {
            intptr_t current_line = sce1.execute(SCI_LINEFROMPOSITION, pos);
            // 0x400 is base level folding
            int fold_level = (sce1.execute(SCI_GETFOLDLEVEL, current_line) & SC_FOLDLEVELNUMBERMASK) - 0x400;
            int prev_fold_level = (sce1.execute(SCI_GETFOLDLEVEL, current_line - 1) & SC_FOLDLEVELNUMBERMASK) - 0x400;
            sce1.execute(SCI_SETLINEINDENTATION, current_line - 1, prev_fold_level * sce1.get_indent_len());
            if (sce1.execute(SCI_GETCHARAT, pos - 3) == '{')
            {
                // line after { requires extra level of indenting
                int other_indent = (prev_fold_level + 1) * sce1.get_indent_len();
                // TODO: Add support for tab based indenting
                std::string to_insert = std::string(other_indent, ' ');
                sce1.execute(SCI_ADDTEXT, to_insert.size(), (LPARAM)to_insert.c_str());
                // Store current position to restore later
                pos = sce1.execute(SCI_GETCURRENTPOS, 0, 0);
                to_insert = "\n";
                sce1.execute(SCI_ADDTEXT, to_insert.size(), (LPARAM)to_insert.c_str());
                int bracket_indent = fold_level * sce1.get_indent_len();
                sce1.execute(SCI_SETLINEINDENTATION, current_line + 1, bracket_indent);
                sce1.execute(SCI_GOTOPOS, pos);
            }
            else if (fold_level > 0)
            {
                int indent = (fold_level)*sce1.get_indent_len();
                std::string to_insert = std::string(indent, ' ');
                sce1.execute(SCI_ADDTEXT, to_insert.size(), (LPARAM)to_insert.c_str());
                sce1.execute(SCI_SETLINEINDENTATION, current_line, indent);
            }
            break;
        }

        default:

            break;
        }
        switch (sce1.execute(SCI_GETCHARAT, sce1.execute(SCI_WORDSTARTPOSITION, pos, true)))
        {
        case 'i': {
            sce1.execute(SCI_AUTOCSHOW, 1, reinterpret_cast<LPARAM>("if include inline int iostream"));
            break;
        }

        case 'c': {
            sce1.execute(SCI_AUTOCSHOW, 1, reinterpret_cast<LPARAM>("case catch char const continue"));
            break;
        }

        case 'd': {
            sce1.execute(SCI_AUTOCSHOW, 1, reinterpret_cast<LPARAM>("default delete do double dynamic_cast"));
            break;
        }

        case 'e': {
            sce1.execute(SCI_AUTOCSHOW, 1, reinterpret_cast<LPARAM>("else enum explicit export extern"));
            break;
        }

        case 'f': {
            sce1.execute(SCI_AUTOCSHOW, 1, reinterpret_cast<LPARAM>("false float for friend"));
            break;
        }

        case 'r': {
            sce1.execute(SCI_AUTOCSHOW, 1, reinterpret_cast<LPARAM>("register reinterpret_cast return"));
            break;
        }

        case 'n': {
            sce1.execute(SCI_AUTOCSHOW, 1, reinterpret_cast<LPARAM>("namespace new nullptr"));
            break;
        }

        case 'p': {
            sce1.execute(SCI_AUTOCSHOW, 1, reinterpret_cast<LPARAM>("private protected public"));
            break;
        }

        case 's': {
            sce1.execute(SCI_AUTOCSHOW, 1, reinterpret_cast<LPARAM>("signed short sizeof static struct switch"));
            break;
        }

        case 't': {
            sce1.execute(SCI_AUTOCSHOW, 1, reinterpret_cast<LPARAM>("templete this try throw typedef typeid typename"));
            break;
        }
        }

        break;
    }
    case SCN_MODIFIED: {
        if ((notification->modificationType & SC_MOD_INSERTCHECK))
        {
            // if (notification->length > 2)
            //{
            //    int pos = notification->position;
            //    int length = notification->length;
            //    char* to_insert = new char[length + 1];
            //    int cur_fold_level = 0;
            //    intptr_t i_new_text = 0;
            //    for (intptr_t i_old_text = 0, size = pos + notification->length, j = 0; i_old_text < length;
            //    i_old_text++)
            //    {
            //        char c = notification->text[i_old_text];
            //        if (c == '\n')
            //        {
            //            to_insert[i_new_text++] = c;
            //            while (isspace(c))
            //            {
            //                i_old_text++;
            //            }
            //            int indent = cur_fold_level * sce1.get_indent_len();
            //            while (indent--)
            //            {
            //                to_insert[i_new_text++] = ' ';
            //            }
            //        }
            //        else
            //        {
            //            if (c == '{' || c == '(')
            //            {
            //                cur_fold_level++;
            //            }
            //            else if (c == '}' || c == ')')
            //            {
            //                cur_fold_level--;
            //            }
            //            to_insert[i_new_text++] = c;
            //        }
            //    }
            //    to_insert[i_new_text] = 0;
            //    MessageBoxA(0, to_insert, " ", 0);
            //    // msg += " Now at " + std::to_string(sce1.execute(SCI_GETCURRENTPOS));
            //    // MessageBox(0, msg.c_str(), "add", 0);
            //}
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