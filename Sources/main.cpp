#include "MainWindow.hpp"
#include "Scintilla.h"
#include "resource.hpp"
#include <cstring>
#include <shlwapi.h>

bool init();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hinstance, LPSTR pCmdLine, int nCmdShow)
{
    if (!init())
    {
        return -1;
    }
    Os::MainWindow main_window;
    if (!main_window.Create(TEXT("Text Editor"),
                            WS_OVERLAPPEDWINDOW | WS_BORDER,
                            WS_EX_APPWINDOW,
                            CW_USEDEFAULT,
                            CW_USEDEFAULT,
                            CW_USEDEFAULT,
                            CW_USEDEFAULT,
                            0,
                            0))
    {
        char error[100];
        sprintf(error, "Error %d: Cannot create main window!", GetLastError());
        MessageBoxA(0, error, "ERROR!", MB_OK);
    }

    ShowWindow(main_window.Window(), nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
