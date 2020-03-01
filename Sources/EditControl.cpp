#include "EditControl.hpp"
#include "Buffer.hpp"
#include "Os.hpp"
#include <ShellAPI.h>
#include <stdexcept>
#include <string>
#include <windows.h>

#ifdef _WIN64
HINSTANCE EditControl::m_hscintilla = LoadLibrary("sci64\\SciLexer.dll");
#else
HINSTANCE EditControl::m_hscintilla = LoadLibrary("sci32\\SciLexer.dll");
#endif

int EditControl::m_num_objects = 0;
const int EditControl::m_line_number_margin = 0;
const int EditControl::m_symbol_margin = 1;
const int EditControl::m_folder_margin = 2;
int EditControl::indent_len = 4;

/* Possible values for markers
SC_MARKNUM_*     | Arrow               Plus/minus           Circle tree                 Box tree
-------------------------------------------------------------------------------------------------------------
FOLDEROPEN       | SC_MARK_ARROWDOWN   SC_MARK_MINUS     SC_MARK_CIRCLEMINUS            SC_MARK_BOXMINUS
FOLDER           | SC_MARK_ARROW       SC_MARK_PLUS      SC_MARK_CIRCLEPLUS             SC_MARK_BOXPLUS
FOLDERSUB        | SC_MARK_EMPTY       SC_MARK_EMPTY     SC_MARK_VLINE                  SC_MARK_VLINE
FOLDERTAIL       | SC_MARK_EMPTY       SC_MARK_EMPTY     SC_MARK_LCORNERCURVE           SC_MARK_LCORNER
FOLDEREND        | SC_MARK_EMPTY       SC_MARK_EMPTY     SC_MARK_CIRCLEPLUSCONNECTED    SC_MARK_BOXPLUSCONNECTED
FOLDEROPENMID    | SC_MARK_EMPTY       SC_MARK_EMPTY     SC_MARK_CIRCLEMINUSCONNECTED   SC_MARK_BOXMINUSCONNECTED
FOLDERMIDTAIL    | SC_MARK_EMPTY       SC_MARK_EMPTY     SC_MARK_TCORNERCURVE           SC_MARK_TCORNER
*/

// clang-format off
const int EditControl::m_marker_array[][NUM_FOLDER_STATE] = {
{SC_MARKNUM_FOLDEROPEN,SC_MARKNUM_FOLDER,SC_MARKNUM_FOLDERSUB,SC_MARKNUM_FOLDERTAIL,SC_MARKNUM_FOLDEREND,SC_MARKNUM_FOLDEROPENMID,
    SC_MARKNUM_FOLDERMIDTAIL},
{SC_MARK_MINUS, SC_MARK_PLUS, SC_MARK_EMPTY, SC_MARK_EMPTY, SC_MARK_EMPTY, SC_MARK_EMPTY, SC_MARK_EMPTY},
{SC_MARK_ARROWDOWN, SC_MARK_ARROW, SC_MARK_EMPTY, SC_MARK_EMPTY, SC_MARK_EMPTY, SC_MARK_EMPTY, SC_MARK_EMPTY},
{SC_MARK_CIRCLEMINUS,SC_MARK_CIRCLEPLUS,SC_MARK_VLINE,SC_MARK_LCORNERCURVE,SC_MARK_CIRCLEPLUSCONNECTED,SC_MARK_CIRCLEMINUSCONNECTED, 
    SC_MARK_TCORNERCURVE},
{SC_MARK_BOXMINUS, SC_MARK_BOXPLUS, SC_MARK_VLINE, SC_MARK_LCORNER, SC_MARK_BOXPLUSCONNECTED, SC_MARK_BOXMINUSCONNECTED, SC_MARK_TCORNER}};
// clang-format on

void EditControl::init(HINSTANCE hInst, HWND hparent)
{
    if (!m_hscintilla)
    {
        throw(std::runtime_error("Cannot load library SciLexer.dll"));
    }
    BaseWindow::init(hInst, hparent);
    m_hwnd = ::CreateWindowEx(WS_EX_CLIENTEDGE,
                              "Scintilla",
                              "",
                              WS_CHILD | WS_VSCROLL | WS_HSCROLL | WS_CLIPCHILDREN,
                              0,
                              0,
                              0,
                              0,
                              m_hparent,
                              NULL,
                              m_hInstance,
                              NULL);
    if (!m_hwnd)
    {
        std::string err_msg = "Error " + std::to_string(GetLastError()) + " Cannot create scintilla window";
        throw std::runtime_error(err_msg);
    }
    m_scintilla_function = (SciFnDirect)::SendMessage(m_hwnd, SCI_GETDIRECTFUNCTION, 0, 0);
    m_scintilla_pointer = (SCINTILLA_PTR)::SendMessage(m_hwnd, SCI_GETDIRECTPOINTER, 0, 0);
    if (!m_scintilla_function || !m_scintilla_pointer)
    {
        throw std::runtime_error("Failed to get direct function to scintilla window.");
    }

    set_cpp_lexer(LangType::L_CPP);
    execute(SCI_SETMARGINMASKN, m_folder_margin, SC_MASK_FOLDERS);
    execute(SCI_SETMARGINWIDTHN, m_folder_margin, 16);
    execute(SCI_SETMARGINSENSITIVEN, m_folder_margin, true);
    execute(SCI_SETMARGINSENSITIVEN, m_symbol_margin, true);
    execute(SCI_SETTABWIDTH, 4);
    execute(SCI_SETINDENT, 0);
    showMargin(m_line_number_margin);
    showMargin(m_folder_margin);
    showMargin(m_symbol_margin);

    execute(SCI_SETPROPERTY, reinterpret_cast<WPARAM>("fold"), reinterpret_cast<LPARAM>("1"));
    execute(SCI_SETPROPERTY, reinterpret_cast<WPARAM>("fold.compact"), reinterpret_cast<LPARAM>("1"));
    execute(SCI_SETPROPERTY, reinterpret_cast<WPARAM>("fold.comment"), reinterpret_cast<LPARAM>("1"));
    execute(SCI_SETPROPERTY, reinterpret_cast<WPARAM>("fold.preprocessor"), reinterpret_cast<LPARAM>("1"));
    //execute(SCI_SETPROPERTY, reinterpret_cast<WPARAM>("styling.within.preprocessor"), reinterpret_cast<LPARAM>("0"));
    execute(SCI_SETPROPERTY, reinterpret_cast<WPARAM>("lexer.cpp.track.preprocessor"), reinterpret_cast<LPARAM>("1"));
    execute(SCI_SETPROPERTY, reinterpret_cast<WPARAM>("lexer.cpp.update.preprocessor"), reinterpret_cast<LPARAM>("1"));
    
    execute(SCI_SETFOLDFLAGS, SC_FOLDFLAG_LINEAFTER_CONTRACTED, 0);
    set_marker_style(FolderStyle::FOLDER_STYLE_BOX);
    execute(SCI_SETAUTOMATICFOLD, SC_AUTOMATICFOLD_CHANGE | SC_AUTOMATICFOLD_CLICK | SC_AUTOMATICFOLD_SHOW, 0);
};

void EditControl::set_style(int style, COLORREF fore, COLORREF back, int size, const char* font) const
{
    execute(SCI_STYLESETFORE, style, fore);
    execute(SCI_STYLESETBACK, style, back);
    if (size >= 1)
        execute(SCI_STYLESETSIZE, style, size);
    if (font)
        execute(SCI_STYLESETFONT, style, reinterpret_cast<LPARAM>(font));
}

void EditControl::set_font(int style, const char* font, bool is_bold, bool is_italic) const
{
    if ((!font) || (strcmp(font, "")))
        execute(SCI_STYLESETFONT, (WPARAM)style, (LPARAM)font);
    if (is_bold)
        execute(SCI_STYLESETBOLD, (WPARAM)style, (LPARAM)is_bold);
    if (is_italic)
        execute(SCI_STYLESETITALIC, (WPARAM)style, (LPARAM)is_italic);
}

const char cInstrWords[] = "if else switch case default break goto return for while do continue typedef sizeof NULL";
const char cppInstrWords[] =
    "if else switch case default break goto return for while do continue typedef sizeof NULL new delete throw try "
    "catch namespace operator this const_cast static_cast dynamic_cast reinterpreter_cast true false null";

const char c_types[] = "void struct union enum char short int long double float signed unsigned const static extern "
                       "auto register volatile";
const char cpp_types[] = "void struct union enum char short int long double float signed unsigned const static extern "
                         "auto register volatile bool class private protected public friend inline template virtual";

void EditControl::set_cpp_lexer(LangType langauge)
{
    std::string cppInstrs;
    std::string cppTypes;
    cppInstrs = cppInstrWords;
    cppTypes = cpp_types;
    
    execute(SCI_SETLEXER, SCLEX_CPP);
    execute(SCI_SETKEYWORDS, 0, (LPARAM)cppInstrs.c_str());
    execute(SCI_SETKEYWORDS, 1, (LPARAM)cppTypes.c_str());
    set_style(STYLE_DEFAULT, grey, white, 10, "Verdana");
    execute(SCI_STYLECLEARALL, 0, 0); // Copies to all other styles.

    set_style(SCE_C_DEFAULT, black, white);                              // 0
    set_style(SCE_C_COMMENT, darkGreen, white, 10, "Comic Sans MS");     // 1
    set_style(SCE_C_COMMENTLINE, darkGreen, white, 10, "Comic Sans MS"); // 2
    set_style(SCE_C_COMMENTDOC, darkGreen, white, 10, "Comic Sans MS");  // 3
    set_style(SCE_C_NUMBER, orange, white, 0, 0);                        // 4
    set_style(SCE_C_WORD, blue, white);                                  // 5
    set_style(SCE_C_WORD2, purple, white);                               // 5
    
    // execute(SCI_STYLESETBOLD, SCE_C_WORD, 1);
    set_style(SCE_C_STRING, RGB(226, 31, 31), white, 0, 0); // 6 // red
    set_style(SCE_C_CHARACTER, RGB(226, 31, 31), white, 0, 0);  // 7
    set_style(SCE_C_PREPROCESSOR, brown, white, 0, 0);          // 9
    set_style(SCE_C_OPERATOR, darkBlue, white, 0, 0);           // 10
    set_style(SCE_C_IDENTIFIER, RGB(116, 18, 33), white);
    //execute(SCI_STYLESETBOLD, SCE_C_OPERATOR, 1);
    set_style(SCE_C_PREPROCESSORCOMMENT, grey, white);
    // set_style(SCE_C_STRINGEOL, darkBlue, white, 0, 0); //12
    // set_style(SCE_C_COMMENTLINEDOC, darkBlue, white, 0, 0); //15
    // set_style(SCE_C_WORD2, darkBlue, white, 0, 0); //16

    return;
}

void EditControl::set_document_language(LangType language)
{
    set_style(STYLE_DEFAULT, black, white, 10, "Verdana");
    execute(SCI_STYLECLEARALL);
    int caretColor = black;
    int caretWidth = 1;
    int selectColorFore = yellow;
    int selectColorBack = grey;
    switch (language)
    {
    case LangType::L_CPP: {
        set_cpp_lexer(language);
        break;
    }
    default:
        set_cpp_lexer(language);
        break;
    }
    // All the global styles should put here
    set_style(STYLE_INDENTGUIDE, liteGrey);
    set_style(STYLE_CONTROLCHAR, liteGrey, red);
    set_style(STYLE_BRACELIGHT, blue, grey);
    set_caret_color_width(caretColor, caretWidth);
    set_selection_color(selectColorFore, selectColorBack);
    execute(SCI_COLOURISE, 0, -1);
}

wchar_t* EditControl::attach_default_doc(int num)
{
    std::wstring title;
    title = DEFAULT_TAB_NAME + std::to_wstring(num);

    // get the doc pointer attached (by default) on the view Scintilla
    Document doc = execute(SCI_GETDOCPOINTER, 0, 0);

    // create the entry for our list
    m_buffer_array.push_back(Buffer(doc, title.c_str()));

    // set current index to 0
    m_current_index = 0;
    return m_buffer_array[m_current_index].m_full_path;
}

int EditControl::get_index(const std::wstring& file_name) const
{
    int index = -1;
    for (size_t size = m_buffer_array.size(), i = 0; i < size; i++)
    {
        if (m_buffer_array[i].m_full_path == file_name)
        {
            index = i;
            break;
        }
    }
    return index;
}

//! \brief this method activates the doc and the corresponding sub tab
//! \brief return the index of previeus current doc
wchar_t* EditControl::activate_document(int index)
{
    // before activating another document, we get the current position
    // from the Scintilla view then save it to the current document
    save_current_pos();
    Position& prevDocPos = m_buffer_array[m_current_index].m_pos;

    // increase current doc ref count to 2
    execute(SCI_ADDREFDOCUMENT, 0, m_buffer_array[m_current_index].m_doc);

    // change the doc, this operation will decrease
    // the ref count of old current doc to 1
    execute(SCI_SETDOCPOINTER, 0, m_buffer_array[index].m_doc);

    // reset all for another doc
    // execute(SCI_CLEARDOCUMENTSTYLE);
    // bool isDocTypeDiff = (m_buffer_array[m_current_index]._lang != m_buffer_array[index]._lang);
    m_current_index = index;

    // if (isDocTypeDiff)
    set_document_language(m_buffer_array[m_current_index].m_language);

    restore_current_pos(prevDocPos);

    // execute(SCI_SETREADONLY, is_current_buf_read_only());

    return m_buffer_array[m_current_index].m_full_path;
}

// this method creates a new doc ,and adds it into
// the end of the doc list and a last sub tab, then activate it
// it returns the name of this created doc (that's the current doc also)
wchar_t* EditControl::create(std::wstring file_name)
{
    Document new_doc = execute(SCI_CREATEDOCUMENT);
    m_buffer_array.push_back(Buffer(new_doc, file_name.c_str()));
    m_buffer_array[m_buffer_array.size() - 1].is_file_read_only();
    return activate_document(int(m_buffer_array.size()) - 1);
}

wchar_t* EditControl::create(int num_new)
{
    wchar_t title[50];
    wcscat(wcscpy(title, DEFAULT_TAB_NAME), std::to_wstring(num_new).c_str());
    return create(title);
}

int EditControl::set_title(std::wstring file_name)
{
    m_buffer_array[m_current_index].set_file_name(file_name.c_str());
    set_document_language(m_buffer_array[m_current_index].m_language);
    return m_current_index;
}

// return the index to close then (argument) the index to activate
int EditControl::close_current(int& to_activate)
{
    int old = m_current_index;
    if (m_current_index == m_buffer_array.size() - 1)
    {
        if (m_current_index)
        {
            m_current_index -= 1;
        }
    }

    // erase the position given document from our list
    m_buffer_array.erase(m_buffer_array.begin() + old);

    // Ref count of closed document owned by Scintilla will be decreased to 0 by SCI_SETDOCPOINTER message
    execute(SCI_SETDOCPOINTER, 0, m_buffer_array[m_current_index].m_doc);
    set_document_language(m_buffer_array[m_current_index].m_language);
    to_activate = m_current_index;

    return old;
}

void EditControl::close_at(int index)
{
    execute(SCI_RELEASEDOCUMENT, 0, m_buffer_array[index].m_doc);
    m_buffer_array.erase(m_buffer_array.begin() + index);
    m_current_index -= (index < m_current_index) ? 1 : 0;
}

void EditControl::remove_all_docs()
{
    for (size_t i = 0, size = m_buffer_array.size(); i < size; i++)
    {
        if (i != m_current_index)
        {
            execute(SCI_RELEASEDOCUMENT, 0, m_buffer_array[i].m_doc);
        }
    }
    m_buffer_array.clear();
}

void EditControl::get_text(char* dest, int start, int end)
{
    Sci_TextRange tr;
    tr.chrg.cpMin = start;
    tr.chrg.cpMax = end;
    tr.lpstrText = dest;
    execute(SCI_GETTEXTRANGE, 0, reinterpret_cast<LPARAM>(&tr));
}
int EditControl::get_line_at(int num_line, char* buf, bool keep_EOL)
{
    int num_char = int(execute(SCI_LINELENGTH, num_line));
    execute(SCI_GETLINE, num_line, (LPARAM)buf);
    if (!keep_EOL)
        num_char -= 2;
    buf[num_char] = '\0';
    return num_char;
}
void EditControl::margin_click(int position, int modifiers)
{
    int line = int(execute(SCI_LINEFROMPOSITION, position, 0));
    int level = int(execute(SCI_GETFOLDLEVEL, line, 0));
    if (level & SC_FOLDLEVELHEADERFLAG)
    {
        if (modifiers & SCMOD_SHIFT)
        {
            // Ensure all children visible
            execute(SCI_SETFOLDEXPANDED, line, 1);
            expand(line, true, true, 100, level);
        }
        else if (modifiers & SCMOD_CTRL)
        {
            if (execute(SCI_GETFOLDEXPANDED, line, 0))
            {
                // Contract this line and all children
                execute(SCI_SETFOLDEXPANDED, line, 0);
                expand(line, false, true, 0, level);
            }
            else
            {
                // Expand this line and all children
                execute(SCI_SETFOLDEXPANDED, line, 1);
                expand(line, true, true, 100, level);
            }
        }
        else
        {
            // Toggle this line
            execute(SCI_TOGGLEFOLD, line, 0);
        }
    }
}

void EditControl::expand(int& line, bool doExpand, bool force, int visLevels, int level)
{
    int lineMaxSubord = int(execute(SCI_GETLASTCHILD, line, level & SC_FOLDLEVELNUMBERMASK));
    line++;
    while (line <= lineMaxSubord)
    {
        if (force)
        {
            if (visLevels > 0)
                execute(SCI_SHOWLINES, line, line);
            else
                execute(SCI_HIDELINES, line, line);
        }
        else
        {
            if (doExpand)
                execute(SCI_SHOWLINES, line, line);
        }
        int levelLine = level;
        if (levelLine == -1)
            levelLine = int(execute(SCI_GETFOLDLEVEL, line, 0));
        if (levelLine & SC_FOLDLEVELHEADERFLAG)
        {
            if (force)
            {
                if (visLevels > 1)
                    execute(SCI_SETFOLDEXPANDED, line, 1);
                else
                    execute(SCI_SETFOLDEXPANDED, line, 0);
                expand(line, doExpand, force, visLevels - 1);
            }
            else
            {
                if (doExpand)
                {
                    if (!execute(SCI_GETFOLDEXPANDED, line, 0))
                        execute(SCI_SETFOLDEXPANDED, line, 1);

                    expand(line, true, force, visLevels - 1);
                }
                else
                {
                    expand(line, false, force, visLevels - 1);
                }
            }
        }
        else
        {
            line++;
        }
    }
}