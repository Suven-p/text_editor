#include "ErrorMessage.hpp"

void display_error(const char *title)
{
    LPVOID lpMsgBuf;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                  NULL,
                  ::GetLastError(),
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                  (LPTSTR)&lpMsgBuf,
                  0,
                  NULL); // Process any inserts in lpMsgBuf.
    MessageBoxA(NULL, (LPTSTR)lpMsgBuf, title, MB_OK | MB_ICONSTOP);
}