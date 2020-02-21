#include "StaticDialog.hpp"
#include "ErrorMessage.hpp"

void StaticDialog::create(int dialogID)
{
    m_hwnd = ::CreateDialogParam(m_hInstance, MAKEINTRESOURCE(dialogID), m_hparent, (DLGPROC)dlgProc, (LPARAM)this);

    if (!m_hwnd)
    {
        display_error("StaticDialog");
        throw int(666);
    }
    display();
}

BOOL CALLBACK StaticDialog::dlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG: {
        StaticDialog *pStaticDlg = (StaticDialog *)(lParam);
        pStaticDlg->m_hwnd = hwnd;
        ::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)lParam);
        pStaticDlg->run_dlgProc(message, wParam, lParam);
        return TRUE;
    }

    default: {
        StaticDialog *pStaticDlg = reinterpret_cast<StaticDialog *>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
        if (!pStaticDlg)
            return FALSE;
        return pStaticDlg->run_dlgProc(message, wParam, lParam);
    }
    }
}