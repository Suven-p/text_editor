#pragma once

#include "BaseWindow.hpp"
#include "Buffer.hpp"
#include "Colors.hpp"
#include "ErrorMessage.hpp"
#include "SciLexer.h"
#include "Scintilla.h"
#include <stdexcept>
#include <string>
#include <vector>

using SCINTILLA_FUNC = int(__cdecl*)(void*, int, int, int);
using SCINTILLA_PTR = void*;
using BUFFER_ARRAY = std::vector<Buffer>;
using CHARACTER_RANGE = Sci_CharacterRange;

#define LINEDRAW_FONT "LINEDRAW.TTF"
constexpr int NUM_FOLDER_STATE = 7;

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
    }
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
        remove_all_docs();
        ::DestroyWindow(m_hwnd);
        m_hwnd = NULL;
    };
    virtual void init(HINSTANCE hInst, HWND hParent);
    LRESULT execute(UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0) const
    {
        return m_scintilla_function(m_scintilla_pointer,
                                    static_cast<int>(Msg),
                                    static_cast<int>(wParam),
                                    static_cast<int>(lParam));
    };
    void set_document_language(LangType language);
    char* attach_default_doc(int num);
    int get_index(const std::string& file_name) const;
    char* activate_document(int index);
    char* create(std::string file_name);
    char* create(int num_new);
    int get_current_index() const
    {
        return m_current_index;
    }
    const char* get_current_title() const
    {
        return m_buffer_array[m_current_index].m_full_path;
    }
    int set_title(std::string file_name);
    int close_current(int& to_activate);
    void close_at(int index);
    void remove_all_docs();
    void get_text(char* dest, int start, int end);
    int EditControl::get_line_at(int num_line, char* buf, bool keep_EOL);
    void set_state(bool is_dirty)
    {
        m_buffer_array[m_current_index].m_is_dirty = is_dirty;
    }
    bool is_current_doc_dirty() const
    {
        return m_buffer_array[m_current_index].m_is_dirty;
    }
    void set_current_doc_read_only(bool is_read_only)
    {
        m_buffer_array[m_current_index].m_is_read_only = is_read_only;
    }
    bool is_current_buf_read_only()
    {
        return m_buffer_array[m_current_index].m_is_read_only;
    }
    void set_language(LangType lang)
    {
        m_buffer_array[m_current_index].m_language = lang;
    }
    bool is_all_docs_clean() const
    {
        for (auto buffer : m_buffer_array)
        {
            if (buffer.m_is_dirty)
                return false;
        }
        return true;
    }
    int get_num_docs() const
    {
        return static_cast<int>(m_buffer_array.size());
    }
    void save_current_pos()
    {
        int top_line = static_cast<int>(execute(SCI_GETFIRSTVISIBLELINE));
        m_buffer_array[m_current_index].m_pos.first_line = top_line;
        m_buffer_array[m_current_index].m_pos.start_pos = int(execute(SCI_GETSELECTIONSTART));
        m_buffer_array[m_current_index].m_pos.end_pos = int(execute(SCI_GETSELECTIONEND));
    }
    void restore_current_pos(const Position& prev_pos)
    {
        int scroll_to_top = 0 - (int(execute(SCI_GETLINECOUNT)) + 1);
        execute(SCI_LINESCROLL, 0, scroll_to_top);
        execute(SCI_LINESCROLL, 0, m_buffer_array[m_current_index].m_pos.first_line);
        execute(SCI_SETSELECTIONSTART, m_buffer_array[m_current_index].m_pos.start_pos);
        execute(SCI_SETSELECTIONEND, m_buffer_array[m_current_index].m_pos.end_pos);
    }
    Buffer& get_buffer(int index)
    {
        if (index >= int(m_buffer_array.size()))
            throw std::range_error("Invalid value for index. Index requested\
				is greater than number of buffers.");
        return m_buffer_array[index];
    }
    int get_current_len() const
    {
        return int(execute(SCI_GETLENGTH));
    }
    CHARACTER_RANGE get_selection() const
    {
        CHARACTER_RANGE crange;
        crange.cpMin = long(execute(SCI_GETSELECTIONSTART));
        crange.cpMax = long(execute(SCI_GETSELECTIONEND));
        return crange;
    }
    LangType get_current_doc_type() const
    {
        return m_buffer_array[m_current_index].m_language;
    }
    void set_caret_color_width(int color, int width = 1) const
    {
        execute(SCI_SETCARETFORE, color);
        execute(SCI_SETCARETWIDTH, width);
    };
    void set_selection_color(int fore, int back) const
    {
        execute(SCI_SETSELFORE, 1, fore);
        execute(SCI_SETSELBACK, 1, back);
    };
    int add_buffer(Buffer& buffer)
    {
        m_buffer_array.push_back(buffer);
        return (int(m_buffer_array.size()) - 1);
    };
    Buffer& get_current_buffer()
    {
        return get_buffer(m_current_index);
    };

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
    void margin_click(int position, int modifiers);
    void set_marker_style(FolderStyle style)
    {
        if (m_folder_style == style)
            return;
        m_folder_style = style;
        for (int i = 0; i < NUM_FOLDER_STATE; i++)
            define_marker(m_marker_array[int(FolderStyle::FOLDER_TYPE)][i],
                          m_marker_array[int(style)][i],
                          white,
                          black);
    }
    FolderStyle get_folder_style()
    {
        return m_folder_style;
    };
    void show_WS(bool show = true)
    {
        execute(SCI_SETVIEWWS, show ? SCWS_VISIBLEALWAYS : SCWS_INVISIBLE);
    };
    void show_EOL(bool show = true)
    {
        execute(SCI_SETVIEWEOL, show);
    };
    void show_invisible_chars(bool show = true)
    {
        show_WS(show);
        show_EOL(show);
    };
    bool is_invisible_chars_shown()
    {
        return bool(execute(SCI_GETVIEWWS));
    };
    void show_indent_guide(bool show = true)
    {
        execute(SCI_SETINDENTATIONGUIDES, (WPARAM)show);
    }
    bool is_indent_guide_shown()
    {
        return bool(execute(SCI_GETINDENTATIONGUIDES));
    }
    void swap_buffer(int dest, int src)
    {
        if (src != dest)
        {
            Buffer to_insert = m_buffer_array.at(src);
            m_buffer_array.erase(m_buffer_array.begin() + src);
            m_buffer_array.insert(m_buffer_array.begin() + dest, to_insert);
        }
    }
    static int get_indent_len()
    {
        return indent_len;
    }
    static void set_indent_len(int len)
    {
        indent_len = len;
    }

  private:
    static int indent_len;
    static HINSTANCE m_hscintilla;
    // Keep track of number of objects. Handle to scintilla.dll will be freed if number of objects reaches zero
    static int m_num_objects;
    static const int m_marker_array[][NUM_FOLDER_STATE];
    FolderStyle m_folder_style;
    SCINTILLA_FUNC m_scintilla_function;
    SCINTILLA_PTR m_scintilla_pointer;
    BUFFER_ARRAY m_buffer_array;
    // TODO: Remove _mSLineDrawFont
    int _MSLineDrawFont;
    int m_current_index;
    void set_style(int style, COLORREF fore, COLORREF back = white, int size = -1, const char* font_name = 0) const;
    void set_font(int style, const char* font_name, bool is_bold = false, bool is_italic = false) const;
    void set_cpp_lexer(LangType type);
    void define_marker(int marker, int markerType, COLORREF fore, COLORREF back)
    {
        execute(SCI_MARKERDEFINE, marker, markerType);
        execute(SCI_MARKERSETFORE, marker, fore);
        execute(SCI_MARKERSETBACK, marker, back);
    };
    void expand(int& line, bool expand, bool force = false, int visLevels = 0, int level = -1);
};