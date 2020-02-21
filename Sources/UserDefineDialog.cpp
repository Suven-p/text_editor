#include "UserDefineDialog.hpp"
#include "EditControl.hpp"

const int g1ColorIndex = 4;
const int g2ColorIndex = 5;
const int g3ColorIndex = 6;
const int g4ColorIndex = 7;

/*
DWORD g_crItems[] = {
    RGB(  0,   0,   0),	RGB( 64,   0,   0),	RGB(128,   0,   0),	RGB(192,   0,   0), RGB(255,   0,   0),
    RGB(  0,  64,   0),	RGB( 64,  64,   0),	RGB(128,  64,   0),	RGB(192,  64,   0), RGB(255,  64,   0),
    RGB(  0,  64,  64),	RGB( 64,  64,  64),	RGB(128,  64,  64),	RGB(192,  64,  64), RGB(255,  64,  64),
    RGB(  0, 128,  64),	RGB( 64, 128,  64),	RGB(128, 128,  64),	RGB(192, 128,  64), RGB(255, 128,  64),
    RGB(  0, 128, 128),	RGB( 64, 128, 128),	RGB(128, 128, 128),	RGB(192, 128, 128), RGB(255, 128, 128),
    RGB(  0, 192, 128),	RGB( 64, 192, 128),	RGB(128, 192, 128),	RGB(192, 192, 128), RGB(255, 192, 128),
    RGB(  0, 192, 192),	RGB( 64, 192, 192),	RGB(128, 192, 192),	RGB(192, 192, 192), RGB(255, 192, 192),
    RGB(  0, 255, 192),	RGB( 64, 255, 192), RGB(255, 255, 192),	RGB(192, 255, 192), RGB(255, 255, 192),
    RGB(  0, 255, 255),	RGB( 64, 255, 255), RGB(255, 255, 255),	RGB(192, 255, 255), RGB(255, 255, 255),
};
*/

DWORD g_crItems[] = {
    RGB(64, 0, 0),    RGB(128, 0, 0),     RGB(128, 64, 64), RGB(255, 0, 0),     RGB(255, 128, 128), RGB(255, 255, 128),
    RGB(255, 255, 0), RGB(255, 128, 64),  RGB(255, 128, 0), RGB(128, 64, 0),    RGB(128, 128, 0),   RGB(128, 128, 64),
    RGB(0, 64, 0),    RGB(0, 128, 0),     RGB(0, 255, 0),   RGB(128, 255, 0),   RGB(128, 255, 128), RGB(0, 255, 128),
    RGB(0, 255, 64),  RGB(0, 128, 128),   RGB(0, 128, 64),  RGB(0, 64, 64),     RGB(128, 128, 128), RGB(64, 128, 128),
    RGB(0, 0, 128),   RGB(0, 0, 255),     RGB(0, 64, 128),  RGB(0, 255, 255),   RGB(128, 255, 255), RGB(0, 128, 255),
    RGB(0, 128, 192), RGB(128, 128, 255), RGB(0, 0, 160),   RGB(0, 0, 64),      RGB(192, 192, 192), RGB(64, 0, 64),
    RGB(64, 0, 64),   RGB(128, 0, 128),   RGB(128, 0, 64),  RGB(128, 128, 192), RGB(255, 128, 192), RGB(255, 128, 255),
    RGB(255, 0, 255), RGB(255, 0, 128),   RGB(128, 0, 255), RGB(64, 0, 128)};

DWORD g_crLiteItems[] = {RGB(255, 255, 192),
                         RGB(255, 255, 160),
                         RGB(192, 255, 192),
                         RGB(192, 255, 255),
                         RGB(160, 255, 255),
                         RGB(255, 192, 255),
                         RGB(255, 160, 255),
                         RGB(255, 192, 192),
                         RGB(192, 192, 160),
                         RGB(192, 192, 255),
                         RGB(160, 192, 192),
                         RGB(192, 192, 192),
                         RGB(224, 224, 224)};

DWORD g_bgColor = 0x00ff00;
DWORD g_fgColor = 0xff0000;

UserDefineDialog::UserDefineDialog() : StaticDialog(), _pScintilla(NULL), _status(UNDOCK), _itemHandlesArray{0}
{
    for (int i = 0; i < nbBoolean; i++)
        _boolList[i] = 'N';
    _boolList[nbBoolean] = '\0';
}

void UserDefineDialog::updateUserDefineData()
{
    //_wordLists
    for (int id = IDC_COMBO_G1_COLOR; id <= IDC_COMBO_G4_COLOR; id++)
    {
        int k = id - IDC_COMBO_G1_COLOR;
        int i = int(::SendDlgItemMessage(m_hwnd, id, CB_GETCURSEL, 0, 0L));
        _wordLists[k]._color = int(::SendDlgItemMessage(m_hwnd, id, CB_GETITEMDATA, i, 0L));
    }

    for (int id = IDC_COMBO_CROCHET_COLOR; id <= IDC_COMBO_DBL_COTE_COLOR; id++)
    {
        int k = id - IDC_COMBO_CROCHET_COLOR;
        int i = int(::SendDlgItemMessage(m_hwnd, id, CB_GETCURSEL, 0, 0L));
        _blockColors[k] = int(::SendDlgItemMessage(m_hwnd, id, CB_GETITEMDATA, i, 0L));
    }
}

void UserDefineDialog::create(int dialogID)
{
    StaticDialog::create(dialogID);
    updateUserDefineData();
    _pScintilla->set_document_language(LangType::L_USER);
}

void UserDefineDialog::changeStyle()
{
    display(false);
    _status = !_status;
    ::SetDlgItemText(m_hwnd, IDC_DOCK_BUTTON, (_status == DOCK) ? "Undock" : "Dock");

    long style = ::GetWindowLongPtr(m_hwnd, GWL_STYLE);
    if (!style)
        ::MessageBox(NULL, "echou GetWindowLong", "", MB_OK);

    style = (_status == DOCK) ? ((style & ~WS_POPUP) & ~DS_MODALFRAME & ~WS_CAPTION) | WS_CHILD
                              : (style & ~WS_CHILD) | WS_POPUP | DS_MODALFRAME | WS_CAPTION;

    long result = ::SetWindowLongPtr(m_hwnd, GWL_STYLE, style);
    if (!result)
        ::MessageBox(NULL, "echou SetWindowLong", "", MB_OK);

    if (_status == DOCK)
        getActualPosSize();
    else
        restorePosSize();

    ::SetWindowPos(m_hwnd,
                   HWND_TOP,
                   0,
                   0,
                   0,
                   0,
                   SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_NOREPOSITION);
    ::SetParent(m_hwnd, (_status == DOCK) ? m_hparent : NULL);
}

BOOL CALLBACK UserDefineDialog::run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG: {
        // Foreground color;
        int colorIndex = 0;
        int nbColor = sizeof(g_crItems) / sizeof(DWORD);
        int nbID = IDC_COMBO_G4_COLOR - IDC_COMBO_G1_COLOR;
        int decalage = nbColor / nbID;

        for (int i = IDC_COMBO_G1_COLOR; i <= IDC_COMBO_G4_COLOR; i++)
        {
            for (int nColor = 0; nColor < nbColor; nColor++)
                ::SendDlgItemMessage(m_hwnd, i, CB_ADDSTRING, 0, (LPARAM)g_crItems[nColor]);

            ::SendDlgItemMessage(m_hwnd, i, CB_SETCURSEL, colorIndex, 0);
            colorIndex += decalage;
        }

        // Background Color
        colorIndex = 0;
        nbColor = sizeof(g_crLiteItems) / sizeof(DWORD);
        nbID = IDC_COMBO_DBL_COTE_COLOR - IDC_COMBO_CROCHET_COLOR;
        decalage = nbColor / nbID;

        for (int id = IDC_COMBO_CROCHET_COLOR; id <= IDC_COMBO_DBL_COTE_COLOR; id++)
        {
            // Add all colors
            for (int nColor = 0; nColor < nbColor; nColor++)
                ::SendDlgItemMessage(m_hwnd, id, CB_ADDSTRING, 0, (LPARAM)g_crLiteItems[nColor]);

            // Select a color
            ::SendDlgItemMessage(m_hwnd, id, CB_SETCURSEL, colorIndex, 0);
            colorIndex += decalage;
        }
        return TRUE;
    }

    case WM_COMMAND: {
        if (HIWORD(wParam) == EN_CHANGE)
        {
            int k = (LOWORD(wParam)) - (IDC_COMBO_G1_EDIT);
            ::GetDlgItemText(m_hwnd, LOWORD(wParam), _wordLists[k]._words, 1024);

            // UpdateUserLexerData();
            // if (_pScintilla->getCurrentDocType() == L_USER)
            _pScintilla->set_document_language(LangType::L_USER);

            return TRUE;
        }
        else if (HIWORD(wParam) == CBN_SELCHANGE)
        {
            switch (LOWORD(wParam))
            {
            case IDC_COMBO_G1_COLOR:
            case IDC_COMBO_G2_COLOR:
            case IDC_COMBO_G3_COLOR:
            case IDC_COMBO_G4_COLOR: {
                int k = (LOWORD(wParam)) - (IDC_COMBO_G1_COLOR);
                int i = int(::SendDlgItemMessage(m_hwnd, LOWORD(wParam), CB_GETCURSEL, 0, 0L));
                _wordLists[k]._color = int(::SendDlgItemMessage(m_hwnd, LOWORD(wParam), CB_GETITEMDATA, i, 0L));

                break;
            }

            case IDC_COMBO_CROCHET_COLOR:
            case IDC_COMBO__PARENTHESE_COLOR:
            case IDC_COMBO_ACCOLADE_COLOR:
            case IDC_COMBO_LOSANGE_COLOR:
            case IDC_COMBO_DBL_COTE_COLOR: {
                int k = (LOWORD(wParam)) - IDC_COMBO_CROCHET_COLOR;
                int i = int(::SendDlgItemMessage(m_hwnd, LOWORD(wParam), CB_GETCURSEL, 0, 0L));
                _blockColors[k] = int(::SendDlgItemMessage(m_hwnd, LOWORD(wParam), CB_GETITEMDATA, i, 0L));
            }
            default:
                break;
            }
            _pScintilla->set_document_language(LangType::L_USER);
            return TRUE;
        }
        else
        {
            switch (wParam)
            {
            case IDC_DOCK_BUTTON: {
                changeStyle();
                ::SendMessage(m_hparent, (_status == DOCK) ? WM_DOCK_USERDEFINE_DLG : WM_UNDOCK_USERDEFINE_DLG, 0, 0);
                return TRUE;
            }
            case IDCANCEL:
                ::SendMessage(m_hparent, WM_CLOSE_USERDEFINE_DLG, 0, 0);
                display(false);
                return TRUE;

            case IDC_CHECK_G1_BOLD:
            case IDC_CHECK_G2_BOLD:
            case IDC_CHECK_G3_BOLD:
            case IDC_CHECK_G4_BOLD: {
                int k = int(wParam) - (IDC_CHECK_G1_BOLD);
                //_isWholeWord = (BST_CHECKED == ::SendMessage(::GetDlgItem(_hSelf, wParam), BM_GETCHECK, 0, 0));
                _wordLists[k]._fontStyle ^= BOLD_MASK;
                _pScintilla->set_document_language(LangType::L_USER);
                return TRUE;
            }

            case IDC_CHECK_G1_ITALIC:
            case IDC_CHECK_G2_ITALIC:
            case IDC_CHECK_G3_ITALIC:
            case IDC_CHECK_G4_ITALIC: {
                int k = int(wParam) - (IDC_CHECK_G1_ITALIC);
                _wordLists[k]._fontStyle ^= ITALIC_MASK;
                _pScintilla->set_document_language(LangType::L_USER);
                return TRUE;
            }

            case IDC_CHECK_CROCHET:
            case IDC_CHECK_LOSANGE:
            case IDC_CHECK_PARENTHESE:
            case IDC_CHECK_ACCOLADE:
                // case IDC_CHECK_DBL_COTE:
                //{
                //	//int k = wParam - (IDC_CHECK_G1_ITALIC);
                //	int index = (wParam == IDC_CHECK_CROCHET) ? INDEX_CROCHET :
                //		((wParam == IDC_CHECK_PARENTHESE) ? INDEX_PARENTHESE :
                //		((wParam == IDC_CHECK_ACCOLADE) ? INDEX_ACCOLADE :
                //			((wParam == IDC_CHECK_LOSANGE) ? INDEX_LOSANGE : INDEX_DBLCOTE)));
                //	bool isChecked = (BST_CHECKED == ::SendMessage(::GetDlgItem(m_hwnd, int(wParam)), BM_GETCHECK, 0,
                // 0)); 	_boolList[index] = isChecked ? 'Y' : 'N';
                // _pScintilla->set_document_language(LangType::L_USER); 	return TRUE;
                //}

            default:
                break;
            }
        }
        return FALSE;
    }

    case WM_MEASUREITEM: {
        LPMEASUREITEMSTRUCT lpmis = (LPMEASUREITEMSTRUCT)lParam;
        lpmis->itemWidth = 10;
        lpmis->itemHeight = 22;
        break;
    }

    case WM_DRAWITEM: {
        HDC hdc;
        RECT rc;
        COLORREF cr;
        HBRUSH hbrush, hbrHighlight, hbrBackground;

        DRAWITEMSTRUCT *pdis;

        pdis = (DRAWITEMSTRUCT *)lParam;
        hdc = pdis->hDC;
        rc = pdis->rcItem;

        // NULL object ?
        if (pdis->itemData == -1)
            return 0;
        // Brushes.
        hbrHighlight = CreateSolidBrush(0xFFFFFF - g_bgColor);
        hbrBackground = CreateSolidBrush(g_bgColor);

        switch (pdis->itemAction)
        {
        case ODA_DRAWENTIRE:
            switch (pdis->CtlID)
            {
            case IDC_COMBO_G1_COLOR:
            case IDC_COMBO_G2_COLOR:
            case IDC_COMBO_G3_COLOR:
            case IDC_COMBO_G4_COLOR:

            case IDC_COMBO_CROCHET_COLOR:
            case IDC_COMBO_LOSANGE_COLOR:
            case IDC_COMBO__PARENTHESE_COLOR:
            case IDC_COMBO_ACCOLADE_COLOR:
            case IDC_COMBO_DBL_COTE_COLOR:

                rc = pdis->rcItem;
                cr = (COLORREF)pdis->itemData;
                FillRect(hdc, &rc, hbrBackground);
                InflateRect(&rc, -1, -1);
                hbrush = CreateSolidBrush((COLORREF)cr);
                FillRect(hdc, &rc, hbrush);
                DeleteObject(hbrush);
                FrameRect(hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
                break;
            }
            // *** FALL THROUGH ***
        case ODA_SELECT:
            rc = pdis->rcItem;
            if (pdis->itemState & ODS_SELECTED)
                hbrush = hbrHighlight;
            else
                hbrush = hbrBackground;
            FrameRect(hdc, &rc, hbrush);
        case ODA_FOCUS:
            break;
        }
        DeleteObject(hbrHighlight);
        DeleteObject(hbrBackground);
        return 0;
    }
    }
    return FALSE;
}