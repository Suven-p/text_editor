#pragma once

#include "Os.hpp"
#include <string>
#include <vector>

#define IDM_NEW 107
#define IDM_OPEN 108
#define IDM_SAVE 110
#define IDM_SAVE_AS 111
#define IDM_EXIT 113

class Menu
{
    std::string m_name;
    std::vector<Menu> m_sub_menu;
    static int num_identifier;

  public:
    Menu(std::string name, std::vector<Menu> sub = std::vector<Menu>()) : m_name(name), m_sub_menu(sub)
    {
    }
    /*static void display(Menu m)
    {
        cout << "Menu " << m.m_name << " has submenus:\n";
        for (auto menu : m.m_sub_menu)
        {
            cout << menu.m_name << endl;
        }
        for (auto menu : m.m_sub_menu)
        {
            if (!menu.m_sub_menu.empty())
            {
                display(menu);
            }
        }
    }*/
    std::string get_name()
    {
        return m_name;
    }
    HMENU init();
};