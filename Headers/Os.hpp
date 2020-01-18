#pragma once
#include <string>
#include <windows.h>

namespace Os
{
std::wstring get_file_name(HWND owner);
void get_current_dir(LPTSTR str, int sz);

} // namespace Os