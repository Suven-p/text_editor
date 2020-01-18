#include <string>
#include "File.hpp"
#include "Os.hpp"

using namespace std;

void File::open_file()
{
    wstring file_name = Os::get_file_name(m_text_window);
    m_file = fstream(file_name);
}