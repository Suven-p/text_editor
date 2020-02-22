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
    char *init(HINSTANCE hInst, HWND parent, EditControl *sci_view)
    {
        TabControl::init(hInst, parent);
        m_sci_view = sci_view;
        return newDocInit();
    };
    char *newDocInit();
    int find(const char *) const;
    char *activate(int index);
    const char *newDoc(const char *fn = NULL);
    const char *newDoc(Buffer &buf);
    char *clickedUpdate();
    const char *close_current();
    const char *closeAllDocs();
    void close_at(int index);
    // void setCurrentTabItem(const char *title, bool isDirty);
    void updateCurrentTabItem(const char *title = NULL);
    void updateTabItem(int index, const char *title = NULL);
    virtual void resize_to(RECT &rc)
    {
        TabControl::resize_to(rc);
        rc.left += margin;
        rc.top += margin;
        rc.right -= 20;
        rc.bottom -= 40;
        m_sci_view->resize_to(rc);
    };

  private:
    static int m_new_title;
    EditControl *m_sci_view;
};