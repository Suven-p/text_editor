#pragma once

#include "BaseWindow.hpp"
class StaticControl : public Os::BaseWindow
{
  public:
    StaticControl() : BaseWindow(){};
    ~StaticControl(){};
    void init(HINSTANCE hInst, HWND parent, int ctrlID)
    {
        BaseWindow::init(hInst, parent);
    };
}