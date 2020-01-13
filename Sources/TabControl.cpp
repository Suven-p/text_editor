#include "TabControl.hpp"
#include "Os.hpp"

BOOL TabControl::Create()
{
    m_hwnd = CreateWindowEx(0,
                            WC_TABCONTROL,
                            TEXT(""),
                            WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | TCS_BUTTONS | TCS_FOCUSONBUTTONDOWN |
                                TCS_OWNERDRAWFIXED,
                            0,
                            0,
                            parent_rect.right,
                            parent_rect.bottom,
                            m_parent_hwnd,
                            NULL,
                            GetModuleHandle(NULL),
                            NULL);
   
    return (m_hwnd != NULL);
}

BOOL TabControl::New_tab(TCHAR *label)
{
    TCITEM tcit;
    tcit.mask = TCIF_TEXT | TCIF_IMAGE;
    tcit.iImage = -1;
    tcit.pszText = label;
    if (TabCtrl_InsertItem(m_hwnd, m_num_tabs, &tcit) == -1)
    {
        return FALSE;
    }
    TabCtrl_SetItem(m_hwnd, m_num_tabs, &tcit);
   return New_edit(m_hwnd);
}

BOOL TabControl::New_edit(HWND parent)
{
    RECT size = Get_size();
    TabCtrl_AdjustRect(m_hwnd, FALSE, &size);
    m_edit_hwnd[m_num_tabs] =
        CreateWindowEx(0,
                       "EDIT",
                       "",
                       WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL|  ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL| ES_AUTOHSCROLL,
                       0,
                       20,
                       size.right,
                       size.bottom,
                       parent,
                       (HMENU)ID_TEXT_EDITOR_EDIT + m_num_tabs,
                       (HINSTANCE)GetModuleHandle(NULL),
                       NULL);
    if (m_edit_hwnd[m_num_tabs] != NULL)
    {
        m_num_tabs++;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
