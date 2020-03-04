#include "EditView.hpp"

#ifndef _WIN32_IE
#define _WIN32_IE 0x0600
#endif //_WIN32_IE

#include "EditControl.hpp"
#include <commctrl.h>
#include <fstream>
#include <shlwapi.h>

constexpr SIZE_T BUFFER_SIZE = 100;

int EditView::m_new_title = 0;

// return the index if name is found in EditView
// otherwise -1
int EditView::find(const wchar_t* name) const
{
    return m_sci_view->get_index(name);
}

wchar_t* EditView::newDocInit()
{
    // create the new entry for this doc
    wchar_t* newTitle = m_sci_view->attach_default_doc(++m_new_title);

    // create a new (the first) sub tab then hightlight it
    TabControl::insert(newTitle);

    return newTitle;
}

const wchar_t* EditView::newDoc(const std::wstring& fn)
{
    wchar_t* complete_name;
    if (fn.empty())
        complete_name = m_sci_view->create(++m_new_title);
    else
        complete_name = m_sci_view->create(fn);
    // for the title of sub tab
    TabControl::insert(PathFindFileNameW(complete_name));
    TabControl::activate(m_num_tabs - 1);
    m_sci_view->set_document_language(LangType::L_CPP);
    return complete_name;
}

const wchar_t* EditView::newDoc(Buffer& buf)
{
    const wchar_t* complete_name = buf.get_file_name();
    int i = m_sci_view->add_buffer(buf);
    m_sci_view->activate_document(i);

    // for the title of sub tab
    TabControl::insert(PathFindFileNameW(complete_name));
    TabControl::activate(m_num_tabs - 1);
    return complete_name;
}

//! \brief this method activates the doc and the corresponding sub tab
//! \brief return the index of previeus current doc
wchar_t* EditView::activate(int index)
{
    TabControl::activate(index);
    return m_sci_view->activate_document(index);
}

// this method updates the doc when user clicks a sub tab
// return Null if the user clicks on an active sub tab,
// otherwize the name of new activated doc
wchar_t* EditView::clickedUpdate()
{
    int indexClicked = int(::SendMessage(m_hwnd, TCM_GETCURSEL, 0, 0));
    if (indexClicked == m_sci_view->get_current_index())
        return NULL;

    return m_sci_view->activate_document(indexClicked);
    m_sci_view->get_focus();
}

const wchar_t* EditView::close_current()
{
    if (m_num_tabs == 1)
    {
        newDoc();
        close_at(0);
    }
    else
    {
        int i2activate;
        int i2close = m_sci_view->close_current(i2activate);

        TabControl::delete_item(i2close);
        TabControl::activate(i2activate);
    }
    return m_sci_view->get_current_title();
}

const wchar_t* EditView::closeAllDocs()
{
    m_sci_view->remove_all_docs();
    TabControl::delete_all();
    m_new_title = 0;
    newDocInit();
    return m_sci_view->get_current_title();
}

void EditView::close_at(int index2Close)
{
    m_sci_view->close_at(index2Close);
    TabControl::delete_item(index2Close);
}

void EditView::updateCurrentTabItem(const std::wstring& title)
{
    int currentIndex = TabCtrl_GetCurSel(m_hwnd);
    updateTabItem(currentIndex, title);
}

void EditView::updateTabItem(int index, const std::wstring& title)
{
    wchar_t str[32];
    TCITEM tie;
    tie.mask = TCIF_TEXT | TCIF_IMAGE;
    tie.pszText = str;
    tie.cchTextMax = (sizeof(str));

    TabCtrl_GetItem(m_hwnd, index, &tie);
    if (title.empty())
        tie.pszText = const_cast<wchar_t*>(title.c_str());

    bool isDirty = (m_sci_view->get_buffer(index)).is_dirty();        // is_current_buf_read_only();
    bool isReadOnly = (m_sci_view->get_buffer(index)).is_read_only(); // getCurrentDocStat();
    tie.iImage = isReadOnly ? REDONLY_IMG_INDEX : (isDirty ? UNSAVED_IMG_INDEX : SAVED_IMG_INDEX);
    int y = TabCtrl_SetItem(m_hwnd, index, &tie);
}

void EditView::file_open()
{
    OPENFILENAMEW ofn = {};          // common dialog box structure
    WCHAR file_name[MAX_PATH] = L""; // buffer for file name
    HANDLE hf;                       // file handle

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = m_hparent;
    ofn.lpstrFile = file_name;
    ofn.nMaxFile = sizeof(file_name) / sizeof(file_name[0]);
    ofn.lpstrFilter = L"Source\0*.CPP\0Header\0*.HPP\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // Display the Open dialog box.

    if (GetOpenFileNameW(&ofn) == TRUE)
    {
        char read_buffer[BUFFER_SIZE] = {0};
        DWORD num_bytes_read = 0;
        HANDLE h_file =
            CreateFileW(ofn.lpstrFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        newDoc(ofn.lpstrFile);
        while (1)
        {
            bool b_result = ReadFile(h_file, read_buffer, BUFFER_SIZE - 1, &num_bytes_read, NULL);
            if (!b_result)
            {
                std::string err_msg = "Error when reading file";
                throw std::runtime_error(err_msg);
            }
            if (b_result && num_bytes_read == 0)
            {
                break;
            }
            m_sci_view->execute(SCI_APPENDTEXT, num_bytes_read, reinterpret_cast<LPARAM>(read_buffer));
        }
    }
}

void EditView::file_save(std::wstring ffile_name)
{
    OPENFILENAMEW ofn = {};          // common dialog box structure
    WCHAR file_name[MAX_PATH] = L""; // buffer for file name
    wcscpy(file_name, ffile_name.c_str());
    HANDLE hf; // file handle

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = m_hparent;
    ofn.lpstrFile = file_name;
    ofn.nMaxFile = sizeof(file_name) / sizeof(file_name[0]);
    ofn.lpstrFilter = L"Source\0*.CPP\0Header\0*.HPP\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_OVERWRITEPROMPT;
    if (ffile_name == L"")
    {
        GetSaveFileNameW(&ofn);
    }
    intptr_t length = m_sci_view->execute(SCI_GETLENGTH);
    char* text = new char[length + 1];
    text[0] = 0;
    m_sci_view->execute(SCI_GETTEXT, length + 1, reinterpret_cast<LPARAM>(text));
    // MessageBox(0, text, "a", 0);
    DWORD num_bytes_written = 0;
    if (ffile_name == L"") {
        HANDLE h_file = CreateFileW(ofn.lpstrFile,
                                    GENERIC_WRITE,
                                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                                    NULL,
                                    CREATE_NEW,
                                    FILE_ATTRIBUTE_NORMAL,
                                    NULL);
        if (h_file == INVALID_HANDLE_VALUE)
        {
            std::string err_msg = "Error " + std::to_string(GetLastError()) + " while creating file";
            MessageBox(0, err_msg.c_str(), "Error", 0);
            throw std::runtime_error(err_msg);
            bool b_result = WriteFile(h_file, text, length, &num_bytes_written, NULL);

        }

    }
    HANDLE h_file = CreateFileW(ofn.lpstrFile,
                                GENERIC_WRITE | GENERIC_READ,
                                FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                NULL,
                                CREATE_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);
    if (h_file == INVALID_HANDLE_VALUE)
    {
        std::string err_msg = "Error " + std::to_string(GetLastError()) + " while creating file for save";
        MessageBox(0, err_msg.c_str(), "Error", 0);
        throw std::runtime_error(err_msg);
    }
    bool b_result = WriteFile(h_file, text, length, &num_bytes_written, NULL);
    delete[] text;
}
