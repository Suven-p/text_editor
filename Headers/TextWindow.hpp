#pragma once
#include "BaseWindow.hpp"
#define ID_EDITCHILD 100

namespace Os
{
    class TextWindow : public Os::BaseWindow<TextWindow>
    {
    public:
        PCSTR  ClassName() const { return "EDIT"; }
        LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    };
}