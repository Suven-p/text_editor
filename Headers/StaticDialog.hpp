#pragma once

#include "BaseWindow.hpp"

class StaticDialog : public Os::BaseWindow
{
  public:
    StaticDialog() : BaseWindow(){};
    ~StaticDialog(){};
    virtual void create(int dialogID);

    virtual bool isCreated() const
    {
        return static_cast<bool>(m_hwnd);
    };

    void destroy()
    {
        ::DestroyWindow(m_hwnd);
    };

  protected:
    static BOOL CALLBACK dlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual BOOL CALLBACK run_dlgProc(UINT message, WPARAM wParam, LPARAM lParam) = 0;
};