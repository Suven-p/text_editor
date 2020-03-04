#include "Menu.hpp"

int Menu::num_identifier = 100;

HMENU Menu::init()
{
    HMENU hmenu = CreatePopupMenu();

    for (auto menu : m_sub_menu)
    {
        if (!menu.m_sub_menu.empty())
        {
            if (!AppendMenu(hmenu, MF_STRING | MF_POPUP, (UINT_PTR)menu.init(), menu.m_name.c_str()))
            {
                std::string err_msg = "Error " + std::to_string(GetLastError());
                MessageBox(0, err_msg.c_str(), "Error", 0);
            }
            menu.init();
        }
        else
        {
            if (menu.m_name == "")
            {
                if (!AppendMenu(hmenu, MF_SEPARATOR, num_identifier++, NULL))
                {
                    std::string err_msg = "Error" + std::to_string(GetLastError());
                    MessageBox(0, err_msg.c_str(), "Error", 0);
                }
            }
            else
            {
                //MessageBox(0, std::to_string(num_identifier).c_str(), menu.m_name.c_str(), 0);
                if (!AppendMenu(hmenu, MF_STRING, num_identifier++, menu.m_name.c_str()))
                {
                    std::string err_msg = "Error" + std::to_string(GetLastError());
                    MessageBox(0, err_msg.c_str(), "Error", 0);
                }
            }
            
        }
    }
    return hmenu;
}