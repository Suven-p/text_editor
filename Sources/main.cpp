#include "MainWindow.hpp"
#include "resource.hpp"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hinstance, LPSTR pCmdLine, int nCmdShow)
{
    
    HMODULE hmod = LoadLibrary("../SciLexer.DLL");
    if (hmod == NULL)
    {
        MessageBox(
            0, "The Scintilla DLL could not be loaded.", "Error loading Scintilla", MB_OK | MB_ICONERROR);
    }
    Os::MainWindow main_window;
    HMENU main_menu = LoadMenu(hinstance, MAKEINTRESOURCE(IDM_TEXT_EDITOR_MainWindow));
    if (!main_window.Create("Text Editor",
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
        sprintf(error, "Error %d", GetLastError());
        MessageBox(0, error, "ERROR!", MB_OK);
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
