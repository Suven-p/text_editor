#include "EditControl.hpp"
#include "Buffer.h"
#include "Os.hpp"
#include <ShellAPI.h>
#include <stdexcept>
#include <string>
#include <windows.h>

// initialize the static variable
int EditControl::m_num_objects = 0;
UserDefineDialog EditControl::_userDefineDlg;

const int EditControl::m_line_number_margin = 0;
const int EditControl::m_symbol_margin = 1;
const int EditControl::m_folder_margin = 2;

/*
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

const int EditControl::m_marker_array[][NB_FOLDER_STATE] = {
    {SC_MARKNUM_FOLDEROPEN,
     SC_MARKNUM_FOLDER,
     SC_MARKNUM_FOLDERSUB,
     SC_MARKNUM_FOLDERTAIL,
     SC_MARKNUM_FOLDEREND,
     SC_MARKNUM_FOLDEROPENMID,
     SC_MARKNUM_FOLDERMIDTAIL},
    {SC_MARK_MINUS, SC_MARK_PLUS, SC_MARK_EMPTY, SC_MARK_EMPTY, SC_MARK_EMPTY, SC_MARK_EMPTY, SC_MARK_EMPTY},
    {SC_MARK_ARROWDOWN, SC_MARK_ARROW, SC_MARK_EMPTY, SC_MARK_EMPTY, SC_MARK_EMPTY, SC_MARK_EMPTY, SC_MARK_EMPTY},
    {SC_MARK_CIRCLEMINUS,
     SC_MARK_CIRCLEPLUS,
     SC_MARK_VLINE,
     SC_MARK_LCORNERCURVE,
     SC_MARK_CIRCLEPLUSCONNECTED,
     SC_MARK_CIRCLEMINUSCONNECTED,
     SC_MARK_TCORNERCURVE},
    {SC_MARK_BOXMINUS,
     SC_MARK_BOXPLUS,
     SC_MARK_VLINE,
     SC_MARK_LCORNER,
     SC_MARK_BOXPLUSCONNECTED,
     SC_MARK_BOXMINUSCONNECTED,
     SC_MARK_TCORNER}};

void EditControl::init(HINSTANCE hInst, HWND hparent)
{
    if (!m_hscintilla)
    {
        throw(std::runtime_error("Cannot load library SciLexer.dll"));
    }
    BaseWindow::init(hInst, hparent);
    RECT parent_rect;
    GetClientRect(m_hparent, &parent_rect);
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
        auto error_code = GetLastError();
        std::string err_msg = "Error " + std::to_string(error_code) + " Cannot create scintilla window";
        throw std::runtime_error(err_msg);
    }
    m_scintilla_function = (SCINTILLA_FUNC)::SendMessage(m_hwnd, SCI_GETDIRECTFUNCTION, 0, 0);
    m_scintilla_pointer = (SCINTILLA_PTR)::SendMessage(m_hwnd, SCI_GETDIRECTPOINTER, 0, 0);
    _userDefineDlg.init(m_hInstance, m_hparent, this);
    if (!m_scintilla_function || !m_scintilla_pointer)
    {
        throw std::runtime_error("Failed to get direct function to scintilla window.");
    }
    execute(SCI_SETMARGINMASKN, m_folder_margin, SC_MASK_FOLDERS);
    execute(SCI_SETMARGINWIDTHN, m_folder_margin, 16);
    execute(SCI_SETMARGINSENSITIVEN, m_folder_margin, true);
    execute(SCI_SETMARGINSENSITIVEN, m_symbol_margin, true);
    showMargin(m_line_number_margin);
    showMargin(m_folder_margin);
    showMargin(m_symbol_margin);

    ::SendMessage(m_hwnd, SCI_SETPROPERTY, (WPARAM)"fold.comment", (LPARAM)"1");
   /*     execute(SCI_SETPROPERTY, reinterpret_cast<WPARAM>("fold"), reinterpret_cast<LPARAM>("1"));
    execute(SCI_SETPROPERTY, reinterpret_cast<WPARAM>("fold.compact"), reinterpret_cast<LPARAM>("1"));
    execute(SCI_SETPROPERTY, reinterpret_cast<WPARAM>("fold.comment"), reinterpret_cast<LPARAM>("1"));
    execute(SCI_SETPROPERTY, reinterpret_cast<WPARAM>("fold.preprocessor"), reinterpret_cast<LPARAM>("1"));
    execute(SCI_SETFOLDFLAGS, SC_FOLDFLAG_LINEAFTER_CONTRACTED, 0);*/
    set_marker_style(FolderStyle::FOLDER_STYLE_BOX);
    execute(SCI_SETAUTOMATICFOLD, SC_AUTOMATICFOLD_CHANGE, 0);
    set_marker_style(m_folder_style);
};

void EditControl::set_style(int style, COLORREF fore, COLORREF back, int size, const char *font) const
{
    execute(SCI_STYLESETFORE, style, fore);
    execute(SCI_STYLESETBACK, style, back);
    if (size >= 1)
        execute(SCI_STYLESETSIZE, style, size);
    if (font)
        execute(SCI_STYLESETFONT, style, reinterpret_cast<LPARAM>(font));
}

void EditControl::set_font(int style, const char *font, bool is_bold, bool is_italic) const
{
    if ((!font) || (strcmp(font, "")))
        execute(SCI_STYLESETFONT, (WPARAM)style, (LPARAM)font);
    if (is_bold)
        execute(SCI_STYLESETBOLD, (WPARAM)style, (LPARAM)is_bold);
    if (is_italic)
        execute(SCI_STYLESETITALIC, (WPARAM)style, (LPARAM)is_italic);
}

const char cInstrWords[] = "if else switch case default break goto return for while do continue typedef sizeof NULL";
const char cppInstrWords[] = "new delete throw try catch namespace operator this const_cast static_cast dynamic_cast "
                             "reinterpreter_cast true false null";

const char cTypeWords[] = "void struct union enum char short int long double float signed unsigned const static extern "
                          "auto register volatile";
const char cppTypeWords[] = "bool class private protected public friend inline template virtual";

void EditControl::set_cpp_lexer(LangType langauge)
{
    std::string cppInstrs;
    std::string cppTypes;

    switch (langauge)
    {
    case LangType::L_C: {
        cppInstrs = cInstrWords;
        cppTypes = cTypeWords;
        break;
    }

    case LangType::L_CPP: {
        cppInstrs = cInstrWords;
        cppTypes = cTypeWords;

        cppInstrs += " ";
        cppInstrs += cppInstrWords;

        cppTypes += " ";
        cppTypes += cppTypeWords;
        break;
    }

    default:
        return;
    }

    execute(SCI_SETLEXER, SCLEX_CPP);
    execute(SCI_SETKEYWORDS, 0, (LPARAM)cppInstrs.c_str());
    execute(SCI_SETKEYWORDS, 1, (LPARAM)cppTypes.c_str());

    // Global default style.
    set_style(STYLE_DEFAULT, black, white, 10, "Verdana");
    execute(SCI_STYLECLEARALL, 0, 0); // Copies to all other styles.

    // C Styles.
    set_style(SCE_C_DEFAULT, black, white);                              // 0
    set_style(SCE_C_COMMENT, darkGreen, white, 10, "Comic Sans MS");     // 1
    set_style(SCE_C_COMMENTLINE, darkGreen, white, 10, "Comic Sans MS"); // 2
    set_style(SCE_C_COMMENTDOC, darkGreen, white, 10, "Comic Sans MS");  // 3
    set_style(SCE_C_NUMBER, orange, white, 0, 0);                        // 4
    set_style(SCE_C_WORD, blue, white);                                  // 5
    set_style(SCE_C_WORD2, purple, white);                               // 5

    execute(SCI_STYLESETBOLD, SCE_C_WORD, 1);
    set_style(SCE_C_STRING, grey, white, 0, 0);    // 6
    set_style(SCE_C_CHARACTER, grey, white, 0, 0); // 7

    set_style(SCE_C_PREPROCESSOR, brown, white, 0, 0); // 9

    set_style(SCE_C_OPERATOR, darkBlue, white, 0, 0); // 10
    execute(SCI_STYLESETBOLD, SCE_C_OPERATOR, 1);

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
    case LangType::L_C:
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
    set_style(STYLE_BRACELIGHT, blue, yellow);
    setCaretColorWidth(caretColor, caretWidth);
    setSelectionColor(selectColorFore, selectColorBack);
    execute(SCI_COLOURISE, 0, -1);
}

char *EditControl::attatchDefaultDoc(int nb)
{
    char title[10];
    char nb_str[4];
    strcat(strcpy(title, UNTITLED_STR), _itoa(nb, nb_str, 10));

    // get the doc pointer attached (by default) on the view Scintilla
    Document doc = execute(SCI_GETDOCPOINTER, 0, 0);

    // create the entry for our list
    m_buffer_array.push_back(Buffer(doc, title));

    // set current index to 0
    m_current_index = 0;
    // execute(SCI_SETSAVEPOINT);
    return m_buffer_array[m_current_index].m_full_path;
}

int EditControl::findDocIndexByName(const char *fn) const
{
    int index = -1;
    for (int i = 0; i < int(m_buffer_array.size()); i++)
    {
        if (!strcmp(m_buffer_array[i].m_full_path, fn))
        {
            index = i;
            break;
        }
    }
    return index;
}

//! \brief this method activates the doc and the corresponding sub tab
//! \brief return the index of previeus current doc
char *EditControl::activate_document(int index)
{
    // before activating another document, we get the current position
    // from the Scintilla view then save it to the current document
    save_current_pos();
    Position &prevDocPos = m_buffer_array[m_current_index].m_pos;

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

    // execute(SCI_SETREADONLY, isCurrentBufReadOnly());

    return m_buffer_array[m_current_index].m_full_path;
}

// this method creates a new doc ,and adds it into
// the end of the doc list and a last sub tab, then activate it
// it returns the name of this created doc (that's the current doc also)
char *EditControl::create(const char *fn)
{
    Document newDoc = execute(SCI_CREATEDOCUMENT);
    m_buffer_array.push_back(Buffer(newDoc, fn));
    m_buffer_array[m_buffer_array.size() - 1].is_file_read_only();
    return activate_document(int(m_buffer_array.size()) - 1);
}

char *EditControl::create(int nbNew)
{
    char title[10];
    char nb[4];
    strcat(strcpy(title, UNTITLED_STR), _itoa(nbNew, nb, 10));
    return create(title);
}

// return the index to close then (argument) the index to activate
int EditControl::closeCurrentDoc(int &i2Activate)
{
    int oldCurrent = m_current_index;

    // if the file 2 delete is the last one
    if (m_current_index == m_buffer_array.size() - 1)
    {
        // if current index is 0, ie. the current is the only one
        if (!m_current_index)
        {
            m_current_index = 0;
        }
        // the current is NOT the only one and it is the last one,
        // we set it to the index which precedes it
        else
            m_current_index -= 1;
    }
    // else the next current index will be the same,
    // we do nothing

    // get the iterator and calculate its position with the old current index value
    BUFFER_ARRAY::iterator posIt = m_buffer_array.begin() + oldCurrent;

    // erase the position given document from our list
    m_buffer_array.erase(posIt);

    // set another document, so the ref count of old active document owned
    // by Scintilla view will be decreased to 0 by SCI_SETDOCPOINTER message
    execute(SCI_SETDOCPOINTER, 0, m_buffer_array[m_current_index].m_doc);

    set_document_language(m_buffer_array[m_current_index].m_language);

    i2Activate = m_current_index;

    return oldCurrent;
}

void EditControl::closeDocAt(int i2Close)
{
    execute(SCI_RELEASEDOCUMENT, 0, m_buffer_array[i2Close].m_doc);

    // get the iterator and calculate its position with the old current index value
    BUFFER_ARRAY::iterator posIt = m_buffer_array.begin() + i2Close;

    // erase the position given document from our list
    m_buffer_array.erase(posIt);

    m_current_index -= (i2Close < m_current_index) ? 1 : 0;
}

void EditControl::removeAllUnusedDocs()
{
    // unreference all docs  from list of Scintilla
    // by sending SCI_RELEASEDOCUMENT message
    for (int i = 0; i < int(m_buffer_array.size()); i++)
        if (i != m_current_index)
            execute(SCI_RELEASEDOCUMENT, 0, m_buffer_array[i].m_doc);

    // remove all docs except the current doc from list
    m_buffer_array.clear();
}

void EditControl::getText(char *dest, int start, int end)
{
    Sci_TextRange tr;
    tr.chrg.cpMin = start;
    tr.chrg.cpMax = end;
    tr.lpstrText = dest;
    execute(SCI_GETTEXTRANGE, 0, reinterpret_cast<LPARAM>(&tr));
}

void EditControl::marginClick(int position, int modifiers)
{
    int lineClick = int(execute(SCI_LINEFROMPOSITION, position, 0));
    int levelClick = int(execute(SCI_GETFOLDLEVEL, lineClick, 0));
    if (levelClick & SC_FOLDLEVELHEADERFLAG)
    {
        if (modifiers & SCMOD_SHIFT)
        {
            // Ensure all children visible
            execute(SCI_SETFOLDEXPANDED, lineClick, 1);
            expand(lineClick, true, true, 100, levelClick);
        }
        else if (modifiers & SCMOD_CTRL)
        {
            if (execute(SCI_GETFOLDEXPANDED, lineClick, 0))
            {
                // Contract this line and all children
                execute(SCI_SETFOLDEXPANDED, lineClick, 0);
                expand(lineClick, false, true, 0, levelClick);
            }
            else
            {
                // Expand this line and all children
                execute(SCI_SETFOLDEXPANDED, lineClick, 1);
                expand(lineClick, true, true, 100, levelClick);
            }
        }
        else
        {
            // Toggle this line
            execute(SCI_TOGGLEFOLD, lineClick, 0);
        }
    }
}

void EditControl::expand(int &line, bool doExpand, bool force, int visLevels, int level)
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