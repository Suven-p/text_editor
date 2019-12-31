#include "MainWindow.hpp"
#include "TextWindow.hpp"

#include <string>
#include <sstream>
#include <windows.h>


using namespace std;
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR pCmdLine, int nCmdShow)
{
    Os::MainWindow main_window;
    bool create_main_win = main_window.Create("Text Editor", WS_OVERLAPPEDWINDOW);

    Os::TextWindow text_window;
    RECT text_window_pos = {};
    GetWindowRect(main_window.Window(), &text_window_pos);
    bool create_text_win = text_window.Create(0,
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
            0,
            0, 20, (text_window_pos.right - text_window_pos.left) -20, text_window_pos.bottom - text_window_pos.top - 60,
            main_window.Window(),
            (HMENU) ID_EDITCHILD
            );

    if (!create_main_win || !create_text_win)
    {
        stringstream ss;
        ss << "Error: " << GetLastError();
        string err = ss.str();
        MessageBox(0, "Failed to create window!\n", err.c_str(), MB_OK);
        return -2;
    }
    HBRUSH brush = CreateSolidBrush(RGB(0, 100, 255));
    SetClassLongPtr(text_window.Window(), GCLP_HBRBACKGROUND, (LONG)brush);
    ShowWindow(main_window.Window(), nCmdShow);
    ShowWindow(text_window.Window(), nCmdShow);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
