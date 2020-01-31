#pragma once
#define WINVER       0x0601
#define _WIN32_WINNT 0x0601
#include <string>
#include <windows.h>
#include <Windows.h>

namespace Os
{
std::wstring get_file_name(HWND owner);
void get_current_dir(LPTSTR str, int sz);

} // namespace Os