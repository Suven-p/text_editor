#pragma once
//#define NTDDI_VERSION 0x06010000
//#define _WIN32_WINNT  NTDDI_VERSION
//#define STRICT

#include <string>
#include <Windows.h>

namespace Os
{
std::wstring get_file_name(HWND owner);
void get_current_dir(LPTSTR str, int sz);
auto get_exe_dir();
} // namespace Os