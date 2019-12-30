#include <string>
#include "File.hpp"
#include "Os.hpp"

using namespace std;

void File::open_file()
{
    string file_name = Os::get_file_name(0);
    m_file = fstream(file_name);
}