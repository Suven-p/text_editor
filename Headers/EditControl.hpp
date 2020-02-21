#pragma once

#include "BaseWindow.hpp"
#include "Buffer.h"
#include "ErrorMessage.hpp"
#include "SciLexer.h"
#include "Scintilla.h"
#include "UserDefineDialog.hpp"
#include "colors.h"
#include <stdexcept>
#include <vector>

using SCINTILLA_FUNC = int (__cdecl *)(void *, int, int, int);
using SCINTILLA_PTR = void *;
using BUFFER_ARRAY = std::vector<Buffer>;
using CHARACTER_RANGE = Sci_CharacterRange;

constexpr int DEF_SCINTILLA = 1000;
#define WM_DOCK_USERDEFINE_DLG   (WM_USER + DEF_SCINTILLA + 1)
#define WM_UNDOCK_USERDEFINE_DLG (WM_USER + DEF_SCINTILLA + 2)
#define WM_CLOSE_USERDEFINE_DLG  (WM_USER + DEF_SCINTILLA + 3)

#define LINEDRAW_FONT "LINEDRAW.TTF"
const int NB_FOLDER_STATE = 7;

enum class FolderStyle
{
    FOLDER_TYPE,
    FOLDER_STYLE_SIMPLE,
    FOLDER_STYLE_ARROW,
    FOLDER_STYLE_CIRCLE,
    FOLDER_STYLE_BOX
};

class EditControl : public Os::BaseWindow
{
  public:
    EditControl()
        : BaseWindow(), m_scintilla_function(NULL), m_scintilla_pointer(NULL), m_current_index(0), _MSLineDrawFont(0),
          m_folder_style(FolderStyle::FOLDER_STYLE_BOX)
    {
        ++m_num_objects;
    };

    virtual ~EditControl()
    {
        --m_num_objects;
        if ((m_num_objects == 0) && (m_hscintilla))
        {
            ::FreeLibrary(m_hscintilla);
        }
        if (_MSLineDrawFont)
        {
            ::RemoveFontResource(LINEDRAW_FONT);
        }
    };
    virtual void destroy()
    {
        removeAllUnusedDocs();
        ::DestroyWindow(m_hwnd);
        m_hwnd = NULL;
    };
    virtual void init(HINSTANCE hInst, HWND hParent);
    LRESULT execute(UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0) const
    {
        return SendMessage(m_hwnd, Msg, wParam, lParam);
        //return m_scintilla_function(m_scintilla_pointer, static_cast<int>(Msg), static_cast<int>wParam), static_cast<int>(lParam));
    };

    void set_document_language(LangType typeDoc);

    void setCurrentDocType(LangType typeDoc)
    {
        if (m_buffer_array[m_current_index].m_language == typeDoc)
            return;
        m_buffer_array[m_current_index].m_language = typeDoc;
        set_document_language(typeDoc);
    };

    char *attatchDefaultDoc(int nb);

    int findDocIndexByName(const char *fn) const;
    char *activate_document(int index);
    char *create(const char *fn);
    char *create(int nbNew);
    int get_current_index() const
    {
        return m_current_index;
    };
    const char *get_current_title() const
    {
        return m_buffer_array[m_current_index].m_full_path;
    };
    int setCurrentTitle(const char *fn)
    {
        m_buffer_array[m_current_index].set_file_name(fn);
        set_document_language(m_buffer_array[m_current_index].m_language);
        return m_current_index;
    };
    int closeCurrentDoc(int &i2Activate);
    void closeDocAt(int i2Close);

    void removeAllUnusedDocs();
    void getText(char *dest, int start, int end);

    void setCurrentDocState(bool isDirty)
    {
        m_buffer_array[m_current_index].m_is_dirty = isDirty;
    };

    bool is_current_doc_dirty() const
    {
        return m_buffer_array[m_current_index].m_is_dirty;
    };

    void setCurrentDocReadOnly(bool isReadOnly)
    {
        m_buffer_array[m_current_index].m_read_only = isReadOnly;
    };

    bool isCurrentBufReadOnly()
    {
        return m_buffer_array[m_current_index].m_read_only;
    };

    void set_language(LangType lang)
    {
        m_buffer_array[m_current_index].m_language = lang;
    };

    bool is_all_docs_clean() const
    {
        for (int i = 0; i < static_cast<int>(m_buffer_array.size()); i++)
            if (m_buffer_array[i].m_is_dirty)
                return false;
        return true;
    };

    int get_num_docs() const
    {
        return static_cast<int>(m_buffer_array.size());
    };

    void save_current_pos()
    {
        int topLine = static_cast<int>(execute(SCI_GETFIRSTVISIBLELINE));
        m_buffer_array[m_current_index].m_pos.first_line = topLine;

        m_buffer_array[m_current_index].m_pos.start_pos = int(execute(SCI_GETSELECTIONSTART));
        m_buffer_array[m_current_index].m_pos.end_pos = int(execute(SCI_GETSELECTIONEND));
    };

    void restore_current_pos(const Position &prevPos)
    {
        int scroll2Top = 0 - (int(execute(SCI_GETLINECOUNT)) + 1);
        execute(SCI_LINESCROLL, 0, scroll2Top);

        execute(SCI_LINESCROLL, 0, m_buffer_array[m_current_index].m_pos.first_line);
        execute(SCI_SETSELECTIONSTART, m_buffer_array[m_current_index].m_pos.start_pos);
        execute(SCI_SETSELECTIONEND, m_buffer_array[m_current_index].m_pos.end_pos);
    };

    Buffer &get_buffer(int index)
    {
        if (index >= int(m_buffer_array.size()))
            throw std::range_error("Invalid value for index. Index requested\
				is greater than number of buffers.");
        return m_buffer_array[index];
    };

    int getLineStrAt(int nbLine, char *buf, bool activatReturn)
    {
        int nbChar = int(execute(SCI_LINELENGTH, nbLine));
        execute(SCI_GETLINE, nbLine, (LPARAM)buf);
        if (!activatReturn)
            // eliminate '\n' (0D 0A)
            nbChar -= 2;
        buf[nbChar] = '\0';
        return nbChar;
    };

    int getCurrentDocLen() const
    {
        return int(execute(SCI_GETLENGTH));
    };

    CHARACTER_RANGE getSelection() const
    {
        CHARACTER_RANGE crange;
        crange.cpMin = long(execute(SCI_GETSELECTIONSTART));
        crange.cpMax = long(execute(SCI_GETSELECTIONEND));
        return crange;
    };

    LangType getCurrentDocType() const
    {
        return m_buffer_array[m_current_index].m_language;
    };

    void doUserDefineDlg(bool willBeShown = true)
    {
        _userDefineDlg.doDialog(willBeShown);
    };

    /*void setKeyWord(int index, const char* words) {
        strcpy(_wordListArray[index], words);
    };*/

    static UserDefineDialog *getUserDefineDlg()
    {
        return &_userDefineDlg;
    };

    // void changeUserDefineDlgStyle() {_userDefineDlg.changeStyle();};

    void setCaretColorWidth(int color, int width = 1) const
    {
        execute(SCI_SETCARETFORE, color);
        execute(SCI_SETCARETWIDTH, width);
    };

    void setSelectionColor(int foreColor, int backColor) const
    {
        execute(SCI_SETSELFORE, 1, foreColor);
        execute(SCI_SETSELBACK, 1, backColor);
    };

    int addBuffer(Buffer &buffer)
    {
        m_buffer_array.push_back(buffer);
        return (int(m_buffer_array.size()) - 1);
    };

    Buffer &getCurrentBuffer()
    {
        return get_buffer(m_current_index);
    };

    void beSwitched()
    {
        _userDefineDlg.setScintilla(this);
    };

    void showMargin()
    {
        execute(SCI_SETMARGINWIDTHN, 0, 32);
    };

    // Margins
    static const int m_line_number_margin;
    static const int m_symbol_margin;
    static const int m_folder_margin;
    void showMargin(int margin, bool is_shown = true)
    {
        execute(SCI_SETMARGINWIDTHN, margin, is_shown ? ((margin == m_line_number_margin) ? 32 : 16) : 0);
    }
    bool is_margin_shown(int margin)
    {
        return static_cast<bool>(execute(SCI_GETMARGINWIDTHN, margin, 0));
    }
    void marginClick(int position, int modifiers);
    void set_marker_style(FolderStyle style)
    {
        if (m_folder_style == style)
            return;
        m_folder_style = style;
        for (int i = 0; i < NB_FOLDER_STATE; i++)
            define_marker(
                m_marker_array[int(FolderStyle::FOLDER_TYPE)][i], m_marker_array[int(style)][i], white, black);
    }

    FolderStyle getFolderStyle()
    {
        return m_folder_style;
    };

    void showWSAndTab(bool willBeShowed = true)
    {
        execute(SCI_SETVIEWWS, willBeShowed ? SCWS_VISIBLEALWAYS : SCWS_INVISIBLE);
    };

    void showEOL(bool willBeShowed = true)
    {
        execute(SCI_SETVIEWEOL, willBeShowed);
    };

    void showInvisibleChars(bool willBeShowed = true)
    {
        showWSAndTab(willBeShowed);
        showEOL(willBeShowed);
    };

    bool isInvisibleCharsShown(bool willBeShowed = true)
    {
        return bool(execute(SCI_GETVIEWWS));
    };

    void showIndentGuideLine(bool willBeShowed = true)
    {
        execute(SCI_SETINDENTATIONGUIDES, (WPARAM)willBeShowed);
    };

    bool isShownIndentGuide()
    {
        return bool(execute(SCI_GETINDENTATIONGUIDES));
    };

    void sortBuffer(int destIndex, int scrIndex)
    {
        // Do nothing if there's no change of the position
        if (scrIndex == destIndex)
            return;

        Buffer buf2Insert = m_buffer_array.at(scrIndex);
        m_buffer_array.erase(m_buffer_array.begin() + scrIndex);
        m_buffer_array.insert(m_buffer_array.begin() + destIndex, buf2Insert);
    };

  private:
    static HINSTANCE m_hscintilla;
    // Keep track of number of objects. Handle to scintilla.dll will be freed if number of objects reaches zero
    static int m_num_objects;
    static UserDefineDialog _userDefineDlg;

    static const int m_marker_array[][NB_FOLDER_STATE];

    FolderStyle m_folder_style;

    SCINTILLA_FUNC m_scintilla_function;
    SCINTILLA_PTR m_scintilla_pointer;
    // the list of docs
    BUFFER_ARRAY m_buffer_array;
    // For the file nfo
    int _MSLineDrawFont;
    int m_current_index;
    void set_style(int style, COLORREF fore, COLORREF back = white, int size = -1, const char *font_name = 0) const;
    void set_font(int style, const char *font_name, bool is_bold = false, bool is_italic = false) const;
    void set_cpp_lexer(LangType type);

    void define_marker(int marker, int markerType, COLORREF fore, COLORREF back)
    {
        execute(SCI_MARKERDEFINE, marker, markerType);
        execute(SCI_MARKERSETFORE, marker, fore);
        execute(SCI_MARKERSETBACK, marker, back);
    };
    void expand(int &line, bool expand, bool force = false, int visLevels = 0, int level = -1);
};