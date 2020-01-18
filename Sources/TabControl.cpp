#include "TabControl.hpp"
#include "Os.hpp"
#include "Scintilla.h"

BOOL Os::TabControl::Create()
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

BOOL Os::TabControl::New_tab(TCHAR *label)
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

BOOL Os::TabControl::New_edit(HWND parent)
{
    RECT size = Get_size();
    TabCtrl_AdjustRect(m_hwnd, FALSE, &size);
    m_edit_hwnd[m_num_tabs] =
        CreateWindowEx(0,
                       TEXT("Scintilla"),
                       TEXT(""),
                       WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_CLIPCHILDREN,
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
        // TODO: Replace hardcoded pixel value with calculated value
        SendMessage(m_edit_hwnd[m_num_tabs], SCI_SETMARGINWIDTHN, 0, 15);
        m_num_tabs++;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
