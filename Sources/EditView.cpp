#include "EditView.hpp"

#ifndef _WIN32_IE
#define _WIN32_IE 0x0600
#endif //_WIN32_IE

#include "EditControl.hpp"

#include <commctrl.h>
#include <shlwapi.h>

int EditView::m_new_title = 0;

// return the index if fn is found in EditView
// otherwise -1
int EditView::find(const char *fn) const
{
    return m_sci_view->findDocIndexByName(fn);
}

char *EditView::newDocInit()
{
    // create the new entry for this doc
    char *newTitle = m_sci_view->attatchDefaultDoc(++m_new_title);

    // create a new (the first) sub tab then hightlight it
    TabControl::insert(newTitle);

    return newTitle;
}

const char *EditView::newDoc(const char *fn)
{
    char *completName;
    if ((!fn) || (!strcmp(fn, "")))
        completName = m_sci_view->create(++m_new_title);
    else
        completName = m_sci_view->create(fn);
    // for the title of sub tab
    TabControl::insert(PathFindFileName(completName));
    TabControl::activate(m_num_tabs - 1);
    m_sci_view->set_document_language(LangType::L_CPP);
    return (const char *)completName;
}

const char *EditView::newDoc(Buffer &buf)
{
    const char *completName = buf.get_file_name();
    int i = m_sci_view->addBuffer(buf);
    m_sci_view->activate_document(i);

    // for the title of sub tab
    TabControl::insert(PathFindFileName(completName));
    TabControl::activate(m_num_tabs - 1);
    return completName;
}

//! \brief this method activates the doc and the corresponding sub tab
//! \brief return the index of previeus current doc
char *EditView::activate(int index)
{
    TabControl::activate(index);
    return m_sci_view->activate_document(index);
}

// this method updates the doc when user clicks a sub tab
// return Null if the user clicks on an active sub tab,
// otherwize the name of new activated doc
char *EditView::clickedUpdate()
{
    int indexClicked = int(::SendMessage(m_hwnd, TCM_GETCURSEL, 0, 0));
    if (indexClicked == m_sci_view->get_current_index())
        return NULL;

    return m_sci_view->activate_document(indexClicked);
    m_sci_view->get_focus();
}

const char *EditView::closeCurrentDoc()
{
    if (m_num_tabs == 1)
    {
        newDoc();
        closeDocAt(0);
    }
    else
    {
        int i2activate;
        int i2close = m_sci_view->closeCurrentDoc(i2activate);

        TabControl::delete_item(i2close);
        TabControl::activate(i2activate);
    }
    return m_sci_view->get_current_title();
}

const char *EditView::closeAllDocs()
{
    m_sci_view->removeAllUnusedDocs();
    TabControl::delete_all();
    m_new_title = 0;
    newDocInit();
    return m_sci_view->get_current_title();
}

void EditView::closeDocAt(int index2Close)
{
    m_sci_view->closeDocAt(index2Close);
    TabControl::delete_item(index2Close);
}

void EditView::updateCurrentTabItem(const char *title)
{
    // char str[32];
    int currentIndex = TabCtrl_GetCurSel(m_hwnd);

    updateTabItem(currentIndex, title);
}

void EditView::updateTabItem(int index, const char *title)
{
    char str[32];
    TCITEM tie;
    tie.mask = TCIF_TEXT | TCIF_IMAGE;
    tie.pszText = str;
    tie.cchTextMax = (sizeof(str));

    TabCtrl_GetItem(m_hwnd, index, &tie);
    if ((title) && (strcmp(title, "")))
        tie.pszText = (char *)title;

    bool isDirty = (m_sci_view->get_buffer(index)).is_dirty();        // isCurrentBufReadOnly();
    bool isReadOnly = (m_sci_view->get_buffer(index)).is_read_only(); // getCurrentDocStat();
    tie.iImage = isReadOnly ? REDONLY_IMG_INDEX : (isDirty ? UNSAVED_IMG_INDEX : SAVED_IMG_INDEX);
    int y = TabCtrl_SetItem(m_hwnd, index, &tie);
}