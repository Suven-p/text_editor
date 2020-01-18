#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <stdlib.h>
#include <cstdio>
#include <mmsystem.h>
#include <CommCtrl.h>

#include "D:\Programming\C++ workspace\text_editor\scintilla\include\Scintilla.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

char text[20];
HWND parent = NULL;
HWND child = NULL;

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     pCmdLine,
	int       nShowCmd)
{

    HMODULE scintilla = LoadLibrary("D:\\Programming\\C++ workspace\\text_editor\\Executable\\SciLexer.dll");
    if (scintilla == NULL)
    {

        sprintf(text, "%s", "failed");
    }
    else
    {
        sprintf(text, "%d", scintilla);
    }

	static TCHAR app_name[] = TEXT("HelloWin");
	WNDCLASSEX   wndclassex;

	ZeroMemory(&wndclassex, sizeof(wndclassex));
	wndclassex.cbSize = sizeof(wndclassex);
	wndclassex.style = CS_HREDRAW | CS_VREDRAW;
	wndclassex.lpfnWndProc = WndProc;
	wndclassex.hInstance = hInstance;
	wndclassex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclassex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclassex.hbrBackground = GetStockBrush(WHITE_BRUSH);
	//wndclassex.lpszMenuName = NULL;
	wndclassex.lpszClassName = app_name;
	//wndclassex.hIconSm = NULL;

	ATOM atom = RegisterClassEx(&wndclassex);
	if (0 == atom)
	{
		return 0;  // premature exit
	}

	HWND hwnd = CreateWindow(
		app_name,               // window class name or atom
		TEXT("The Hello Program"),  // window caption
		WS_OVERLAPPEDWINDOW,        // window style
		CW_USEDEFAULT,              // initial x position
		CW_USEDEFAULT,              // initial y position
		CW_USEDEFAULT,              // initial x size
		CW_USEDEFAULT,              // initial y size
		NULL,                       // parent window handle
		NULL,                       // window menu handle
		hInstance,                  // program instance handle
		NULL);                      // creation parameters
	if (0 == hwnd)
	{
		return 0;  // premature exit
	}
	parent = hwnd;

	ShowWindow(hwnd, nShowCmd);
	if (0 == UpdateWindow(hwnd))
	{
		return 0;  // premature exit
	}
    HWND hwndScintilla = child;
	//SendMessage(hwndScintilla,SCI_CREATEDOCUMENT, 0, 0);
	MSG  msg;
	BOOL ret;  // three states: -1, 0 or non-zero

	while (0 != (ret = GetMessage(&msg, NULL, 0, 0)))
	{
		if (-1 == ret)
		{
			// handle the error and/or exit
			// for error call GetLastError();
			return 0;
		}
		else
		{
			TranslateMessage(&msg);
			(void)DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;  // WM_QUIT
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
    case WM_CREATE:
        RECT size;
        GetClientRect(hwnd, &size);
        child = CreateWindowEx(0,
                       "Scintilla",
                       "",
                       WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL|  ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL| ES_AUTOHSCROLL,
                       0,
                       20,
                       size.right,
                       size.bottom,
                       hwnd,
                       (HMENU)1,
                       (HINSTANCE)GetModuleHandle(NULL),
                       NULL);
		if (!child)
        {
            MessageBox(hwnd, "Fail cw", "fail", MB_OK | MB_ICONASTERISK);
        }
        ShowWindow(child, SW_SHOW);
        return 0;
//	case WM_PAINT:
//	{
//		PAINTSTRUCT ps;
//		HDC hdc = BeginPaint(hwnd, &ps);
//		RECT rect;
//		(void)GetClientRect(hwnd, &rect);
//
//		(void)DrawText(hdc, text, -1, &rect,
//			DT_SINGLELINE | DT_CENTER | DT_VCENTER);
//		EndPaint(hwnd, &ps);
//	}
	return 0;  // message processed

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;  // message processed

	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
}
