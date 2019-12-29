#pragma once

#include "BaseWindow.hpp"

namespace Os
{
    class MainWindow : public Os::BaseWindow<MainWindow>
    {
    public:
        PCSTR  ClassName() const { return "Sample Window Class"; }
        LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    };
}