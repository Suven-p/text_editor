#include "EditControl.hpp"
#include "parameters.hpp"
#include <CommCtrl.h>

bool Os::EditControl::create(int num_tab)
{
    RECT size = {0};
    GetClientRect(m_parent, &size);
    TabCtrl_AdjustRect(m_parent, FALSE, &size);
    RECT tab_size;
    int selected = TabCtrl_GetCurSel(m_parent);
    TabCtrl_GetItemRect(m_hwnd, selected, &tab_size);
    int tab_height = tab_size.bottom - tab_size.top;
    m_hwnd = CreateWindowEx(0,
                            class_name(),
                            TEXT(""),
                            WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_CLIPCHILDREN | WS_BORDER,
                            0,
                            tab_height,
                            size.right,
                            size.bottom - 25,
                            m_parent,
                            (HMENU)ID_TEXT_EDITOR_EDIT,
                            (HINSTANCE)GetModuleHandle(NULL),
                            NULL);
    if (m_hwnd == NULL)
    {
        return false;
    }
    direct_function = (int(__cdecl *)(void *, int, int, int))SendMessage(m_hwnd, SCI_GETDIRECTFUNCTION, 0, 0);
    direct_pointer = (void *)SendMessage(m_hwnd, SCI_GETDIRECTPOINTER, 0, 0);
    // TODO: Replace hardcoded pixel value with calculated value
    SendMsg(SCI_SETMARGINS, 1, 0);
    SendMsg(SCI_SETMARGINWIDTHN, 0, 30);
    SendMsg(SCI_STYLESETBACK, STYLE_DEFAULT, Settings::Styles::externc_back_color);
    SendMsg(SCI_STYLESETFORE, STYLE_DEFAULT, Settings::Styles::externc_fore_default);
    SendMsg(SCI_SETCARETFORE, Settings::Styles::externc_cursor, 0);
    SendMsg(SCI_STYLECLEARALL, NULL, NULL);
    SendMsg(SCI_STYLESETFORE, STYLE_LINENUMBER, Settings::Styles::externc_line_number);

    return true;
}

LPCTSTR Os::EditControl::class_name() const
{
    return TEXT("Scintilla");
}

HWND Os::EditControl::window() const
{
    return m_hwnd;
}

int Os::EditControl::SendMsg(int msg, int param1, int param2)
{
    return direct_function(direct_pointer, msg, param1, param2);
}