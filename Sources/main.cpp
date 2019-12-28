#include "MainWindow.hpp"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR pCmdLine, int nCmdShow)
{
    Os::MainWindow main_window;

    if (!main_window.Create("Text Editor", WS_OVERLAPPEDWINDOW))
    {
        return 0;
    }

    ShowWindow(main_window.Window(), nCmdShow);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
