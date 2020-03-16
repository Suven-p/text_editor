#include "MainWindow.hpp"
#include "Scintilla.h"
#include "Trie.hpp"
#include "resource.hpp"
#include <cstring>
#include <shlwapi.h>

bool init();

bool Os::BaseWindow::use_dark_mode = false;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR pCmdLine, _In_ int nCmdShow)
{
    if (!init())
    {
        return -1;
    }
    Os::MainWindow main_window(hInstance);
    main_window.create(TEXT("Text Editor"),
                       WS_OVERLAPPEDWINDOW,
                       WS_EX_APPWINDOW,
                       CW_USEDEFAULT,
                       CW_USEDEFAULT,
                       CW_USEDEFAULT,
                       CW_USEDEFAULT,
                       0,
                       0);

    ShowWindow(main_window.window(), nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}