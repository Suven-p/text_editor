#include "TabControl.hpp"
#include <stdexcept>

const COLORREF blue = RGB(0, 0, 0xFF);
const COLORREF black = RGB(0, 0, 0);
const COLORREF white = RGB(0xFF, 0xFF, 0xFF);
const COLORREF grey = RGB(128, 128, 128);

constexpr int IDC_DRAG_TAB = 1404;
constexpr int IDC_DRAG_INTERDIT_TAB = 1405;
constexpr int IDC_DRAG_PLUS_TAB = 1406;

bool TabControl::m_enable_dragdrop = true;

void TabControl::init(HINSTANCE hInst, HWND parent)
{
    // Load Common Control dll(Comctl32.dll)
    BaseWindow::init(hInst, parent);
    INITCOMMONCONTROLSEX icce;
    icce.dwSize = sizeof(icce);
    icce.dwICC = ICC_TAB_CLASSES;
    InitCommonControlsEx(&icce);

    int style =
        WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE | TCS_FOCUSNEVER | TCS_TABS | TCS_OWNERDRAWFIXED;
    RECT parent_client_rect;
    GetClientRect(m_hparent, &parent_client_rect);
    m_hwnd = ::CreateWindowExW(TCS_EX_FLATSEPARATORS,
                              WC_TABCONTROLW,
                              L"",
                              style,
                              0,
                              0,
                              parent_client_rect.right,
                              30,
                              m_hparent,
                              NULL,
                              m_hInstance,
                              0);
    if (!m_hwnd)
    {
        throw std::runtime_error("Creating TabControl failed!");
    }
    ::SetWindowLongPtrW(m_hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    m_default_proc =
        reinterpret_cast<WNDPROC>(::SetWindowLongPtrW(m_hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(tabbar_proc)));
    if (!m_default_proc)
    {
        // ERROR_ABANDONED_WAIT_0
        auto error_code = GetLastError();
        std::string err_msg = "Error " + std::to_string(error_code) + " Cannot create tabcontrol";
        throw std::runtime_error(err_msg);
    }
    ::SetCursor(::LoadCursor(m_hInstance, IDC_ARROW));
    ::SendMessageW(m_hwnd, TCM_SETPADDING, 0, MAKELPARAM(6, 5));
}

int TabControl::insert(const std::wstring &tab_name)
{
    TCITEMW tie;
    tie.mask = TCIF_TEXT | TCIF_IMAGE;
    int index = -1;
    if (m_has_img)
        index = 0;
    tie.iImage = index;
    tie.pszText = const_cast<wchar_t *>(tab_name.c_str());
    return int(::SendMessageW(m_hwnd, TCM_INSERTITEM, m_num_tabs++, reinterpret_cast<LPARAM>(&tie)));
}

void TabControl::resize_to(RECT &rc)
{
    display(rc.right > 10);
    BaseWindow::resize_to(rc);
    TabCtrl_AdjustRect(m_hwnd, FALSE, &rc);
}

void TabControl::activate(int index)
{
    ::SendMessageW(m_hwnd, TCM_SETCURSEL, index, 0);
};

void TabControl::delete_item(int index)
{
    ::SendMessage(m_hwnd, TCM_DELETEITEM, index, 0);
    m_num_tabs--;
};

void TabControl::delete_all()
{
    ::SendMessage(m_hwnd, TCM_DELETEALLITEMS, 0, 0);
    m_num_tabs = 0;
};

void TabControl::set_image(HIMAGELIST himg)
{
    m_has_img = true;
    ::SendMessage(m_hwnd, TCM_SETIMAGELIST, 0, (LPARAM)himg);
};

LRESULT TabControl::handle_message(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_ERASEBKGND: {
        if (use_dark_mode)
        {
            HBRUSH hb = ::CreateSolidBrush(RGB(0, 0, 0));
            RECT r1;
            GetClientRect(m_hwnd, &r1);
            FillRect(reinterpret_cast<HDC>(wParam), &r1, hb);
            ::DeleteObject((HGDIOBJ)hb);
            return 1;
        }
        else
        {
            break;
        }
    }
    case WM_LBUTTONDOWN: {
        ::CallWindowProc(m_default_proc, hwnd, msg, wParam, lParam);
        if (m_enable_dragdrop)
        {
            m_tab_dragged = ::SendMessage(m_hwnd, TCM_GETCURSEL, 0, 0);
            m_source_tab = m_tab_dragged;
            POINT point;
            point.x = LOWORD(lParam);
            point.y = HIWORD(lParam);
            if (::DragDetect(hwnd, point))
            {
                m_is_dragged = true;
                ::SetCapture(hwnd);
                return TRUE;
            }
            break;
        }
        else
            return TRUE;
    }

    case WM_MOUSEMOVE: {
        if (m_is_dragged)
        {
            POINT p;
            p.x = LOWORD(lParam);
            p.y = HIWORD(lParam);
            exchangeItemData(p);
            ::GetCursorPos(&m_drag_point);
            dragging_cursor(m_drag_point);
            return TRUE;
        }
        break;
    }

    case WM_LBUTTONUP: {
        if (m_is_dragged)
        {
            if (::GetCapture() == m_hwnd)
                ::ReleaseCapture();
            NMHDR nmhdr;
            nmhdr.hwndFrom = m_hwnd;
            nmhdr.code = m_is_dragging_inside ? TCN_TABDROPPED : TCN_TABDROPPEDOUTSIDE;
            nmhdr.idFrom = reinterpret_cast<UINT_PTR>(this);
            ::SendMessage(m_hparent, WM_NOTIFY, 0, reinterpret_cast<LPARAM>(&nmhdr));
            return TRUE;
        }
        break;
    }

    case WM_CAPTURECHANGED: {
        if (m_is_dragged)
        {
            m_is_dragged = false;
            return TRUE;
        }
        break;
    }

    case WM_DRAWITEM: {
        draw_item((DRAWITEMSTRUCT *)lParam);
        return TRUE;
    }

    case WM_KEYDOWN: {
        if (wParam == VK_LCONTROL)
            ::SetCursor(::LoadCursor(m_hInstance, MAKEINTRESOURCE(IDC_DRAG_PLUS_TAB)));
        return TRUE;
    }
    default:
        break;
    }
    return ::CallWindowProc(m_default_proc, hwnd, msg, wParam, lParam);
}

void TabControl::draw_item(DRAWITEMSTRUCT *dis)
{
    RECT rect = dis->rcItem;
    rect.bottom += 4;
    int num_tab = dis->itemID;
    if (num_tab < 0)
    {
        return;
    }
    bool isSelected = (num_tab == ::SendMessage(m_hwnd, TCM_GETCURSEL, 0, 0));

    wchar_t label[64];
    wchar_t *plabel = nullptr;
    label[63] = '\0';
    TCITEMW tci;
    tci.mask = TCIF_TEXT | TCIF_IMAGE;
    tci.pszText = label;
    tci.cchTextMax = 63;

    if (!::SendMessageW(m_hwnd, TCM_GETITEM, num_tab, reinterpret_cast<LPARAM>(&tci)))
    {
        ::MessageBoxA(NULL, "! TCM_GETITEM", "", MB_OK);
        // return ::CallWindowProc(m_default_proc, hwnd, msg, wParam, lParam);
    }
    plabel = tci.pszText;

    HDC hDC = dis->hDC;

    int nSavedDC = ::SaveDC(hDC);

    // For some bizarre reason the rcItem you get extends above the actual
    // drawing area. We have to workaround this "feature".
    // rect.top += ::GetSystemMetrics(SM_CYEDGE);

    ::SetBkMode(hDC, TRANSPARENT);
    COLORREF bk_color;
    if (use_dark_mode)
    {
        bk_color = RGB(0, 0, 0);
    }
    else
    {
        bk_color = ::GetSysColor(COLOR_BTNFACE);
    }
    HBRUSH hBrush = ::CreateSolidBrush(bk_color);
    ::FillRect(hDC, &rect, hBrush);
    ::DeleteObject((HGDIOBJ)hBrush);

    if (isSelected)
    {
        RECT barRect = rect;
        barRect.bottom = 6;

        hBrush = ::CreateSolidBrush(RGB(250, 170, 60)); // Orange
        ::FillRect(hDC, &barRect, hBrush);
        ::DeleteObject((HGDIOBJ)hBrush);
    }
    // Draw image
    HIMAGELIST hImgLst = (HIMAGELIST)::SendMessage(m_hwnd, TCM_GETIMAGELIST, 0, 0);

    if (hImgLst && tci.iImage >= 0)
    {
        SIZE size;
        ::GetTextExtentPointW(hDC, L"  ", 2, &size);
        rect.left += size.cx; // Margin

        // Get height of image so we
        IMAGEINFO info;

        ImageList_GetImageInfo(hImgLst, tci.iImage, &info);

        RECT &imageRect = info.rcImage;
        int yPos = (rect.top + (rect.bottom - rect.top) / 2 + 1) - (imageRect.bottom - imageRect.top) / 2;

        ImageList_Draw(hImgLst, tci.iImage, hDC, rect.left, yPos, isSelected ? ILD_TRANSPARENT : ILD_SELECTED);
        rect.left += imageRect.right - imageRect.left;
    }

    if (isSelected)
    {
        COLORREF tabtext_color;
        if (use_dark_mode)
        {
            tabtext_color = RGB(255, 255, 255);
        }
        else
        {
            tabtext_color = RGB(0, 0, 0);
        }
        ::SetTextColor(hDC, tabtext_color);

        // pDC->SelectObject(&m_SelFont);
        rect.top -= ::GetSystemMetrics(SM_CYEDGE);
        ::DrawTextW(hDC, plabel, wcslen(plabel), &rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
    }
    else
    {
        COLORREF _unselectedColor = grey;
        ::SetTextColor(hDC, _unselectedColor);
        // pDC->SelectObject(&m_UnselFont);
        ::DrawTextW(hDC, plabel, wcslen(plabel), &rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
    }
    ::RestoreDC(hDC, nSavedDC);
}

void TabControl::dragging_cursor(POINT screenPoint)
{
    HWND hWin = ::WindowFromPoint(screenPoint);
    if (m_hwnd == hWin)
        ::SetCursor(::LoadCursor(NULL, IDC_ARROW));
    else
    {
        wchar_t className[256];
        ::GetClassName(hWin, className, 256);
        if ((!wcscmp(className, L"Scintilla")) || (!wcscmp(className, WC_TABCONTROLW)))
        {
            if (::GetKeyState(VK_LCONTROL) & 0x80000000)
                ::SetCursor(::LoadCursor(m_hInstance, MAKEINTRESOURCE(IDC_DRAG_PLUS_TAB)));
            else
                ::SetCursor(::LoadCursor(m_hInstance, MAKEINTRESOURCE(IDC_DRAG_TAB)));
        }
        else
            ::SetCursor(::LoadCursor(m_hInstance, MAKEINTRESOURCE(IDC_DRAG_INTERDIT_TAB)));
    }
}

void TabControl::exchangeItemData(POINT point)
{
    TCHITTESTINFO hitinfo;
    hitinfo.pt.x = point.x;
    hitinfo.pt.y = point.y;

    // Find the destination tab...
    unsigned int nTab = ::SendMessage(m_hwnd, TCM_HITTEST, 0, (LPARAM)&hitinfo);

    // The position is over a tab.
    if (hitinfo.flags != TCHT_NOWHERE)
    {
        m_is_dragging_inside = true;

        if (nTab != m_tab_dragged)
        {
            // 1. set to focus
            ::SendMessage(m_hwnd, TCM_SETCURSEL, nTab, 0);

            // 2. shift their data, and insert the source
            TCITEM itemData_nDraggedTab, itemData_shift;
            itemData_nDraggedTab.mask = itemData_shift.mask = TCIF_IMAGE | TCIF_TEXT;
            wchar_t str1[256];
            wchar_t str2[256];

            itemData_nDraggedTab.pszText = str1;
            itemData_nDraggedTab.cchTextMax = (sizeof(str1));

            itemData_shift.pszText = str2;
            itemData_shift.cchTextMax = (sizeof(str2));

            ::SendMessage(m_hwnd, TCM_GETITEM, m_tab_dragged, reinterpret_cast<LPARAM>(&itemData_nDraggedTab));

            if (m_tab_dragged > nTab)
            {
                for (UINT i = m_tab_dragged; i > nTab; i--)
                {
                    ::SendMessage(m_hwnd, TCM_GETITEM, i - 1, reinterpret_cast<LPARAM>(&itemData_shift));
                    ::SendMessage(m_hwnd, TCM_SETITEM, i, reinterpret_cast<LPARAM>(&itemData_shift));
                }
            }
            else
            {
                for (int i = m_tab_dragged; i < nTab; i++)
                {
                    TabCtrl_GetItem(m_hwnd, i+1, &itemData_shift);
                    TabCtrl_SetItem(m_hwnd, i, &itemData_shift);
                }
            }
            //
            ::SendMessage(m_hwnd, TCM_SETITEM, nTab, reinterpret_cast<LPARAM>(&itemData_nDraggedTab));

            // 3. update the current index
            m_tab_dragged = nTab;
        }
    }
    else
    {
        //::SetCursor(::LoadCursor(_hInst, MAKEINTRESOURCE(IDC_DRAG_TAB)));
        m_is_dragging_inside = false;
    }
}

void TabControl::destroy()
{
    if (m_hfont)
        DeleteObject(m_hfont);
    ::DestroyWindow(m_hwnd);
    m_hwnd = NULL;
};