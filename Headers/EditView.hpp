#pragma once

#include "EditControl.hpp"
#include "TabControl.hpp"
//#include "ImageListSet.h"

const int SAVED_IMG_INDEX = 0;
const int UNSAVED_IMG_INDEX = 1;
const int REDONLY_IMG_INDEX = 2;

class EditView : public TabControl
{
  public:
    EditView() : TabControl(), m_sci_view(NULL)
    {
    }
    virtual ~EditView()
    {
        destroy();
    }
    virtual void destroy()
    {
        TabControl::destroy();
    };
    wchar_t* init(HINSTANCE hInst, HWND parent, EditControl* sci_view)
    {
        TabControl::init(hInst, parent);
        m_sci_view = sci_view;
        return newDocInit();
    };
    wchar_t* newDocInit();
    int find(const wchar_t*) const;
    wchar_t* activate(int index);
    const wchar_t* newDoc(const std::wstring& name = L"");
    const wchar_t* newDoc(Buffer& buf);
    wchar_t* clickedUpdate();
    const wchar_t* close_current();
    const wchar_t* closeAllDocs();
    void close_at(int index);
    // void setCurrentTabItem(const char *title, bool isDirty);
    void updateCurrentTabItem(const std::wstring& title = L"");
    void updateTabItem(int index, const std::wstring& title = NULL);
    virtual void resize_to(RECT& rc)
    {
        TabControl::resize_to(rc);
        rc.left += margin;
        rc.top += margin;
        rc.right -= 20;
        rc.bottom -= 40;
        m_sci_view->resize_to(rc);
    };
    void file_open();
    void file_save(std::wstring ffile_name);

  private:
    static int m_new_title;
    EditControl* m_sci_view;
};