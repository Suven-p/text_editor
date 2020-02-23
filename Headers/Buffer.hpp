#pragma once

#include "Scintilla.h"
#include <shlwapi.h>
#include <sys/stat.h>
#include <sys/types.h>

const char UNTITLED_STR[] = "New ";
using Document = sptr_t;
enum class LangType
{
    L_CPP,
    L_TXT
};

struct Position
{
    int first_line;
    int start_pos;
    int end_pos;
};

class Buffer
{
    friend class EditControl;

  public:
    Buffer() = delete;
    Buffer(Document doc, LPCTSTR fileName) : m_doc(doc), m_is_dirty(false), m_is_read_only(false)
    {
        m_full_path = new char[MAX_PATH];
        m_pos.first_line = 0;
        m_pos.start_pos = 0;
        m_pos.end_pos = 0;
        set_file_name(fileName);
    };
    Buffer(const Buffer& buf)
        : m_is_dirty(buf.m_is_dirty), m_doc(buf.m_doc), m_language(buf.m_language), m_is_read_only(buf.m_is_read_only),
          m_pos(buf.m_pos)
    {
        m_full_path = new char[MAX_PATH];
        strcpy(m_full_path, buf.m_full_path);
    };
    Buffer& operator=(const Buffer& buf)
    {
        if (this != &buf)
        {
            m_is_dirty = buf.m_is_dirty;
            m_doc = buf.m_doc;
            m_language = buf.m_language;
            m_is_read_only = buf.m_is_read_only;
            m_pos = buf.m_pos;
            strcpy(m_full_path, buf.m_full_path);
        }
        return *this;
    }
    Buffer(Buffer &&buf) noexcept
        : m_is_dirty(std::move(buf.m_is_dirty)), m_doc(std::move(buf.m_doc)),
          m_is_read_only(std::move(buf.m_is_read_only)), m_pos(std::move(buf.m_pos))
    {
        m_language = buf.m_language;
        m_full_path = buf.m_full_path;
        buf.m_full_path = nullptr;
    }
    Buffer &operator=(Buffer &&buf) noexcept
    {
        if (this != &buf)
        {
            if (m_full_path)
                delete[] m_full_path;
            m_is_dirty = std::move(buf.m_is_dirty);
            m_doc = std::move(buf.m_doc);
            m_language = std::move(buf.m_language);
            m_is_read_only = std::move(buf.m_is_read_only);
            m_pos = std::move(buf.m_pos);
            m_full_path = buf.m_full_path;
            buf.m_full_path = nullptr;
        }
        return *this;
    }
    ~Buffer()
    {
        if (m_full_path)
        {
            delete[] m_full_path;
        }
    }
    // TODO: Add txt file
    void set_file_name(const char *fn)
    {
        strcpy(m_full_path, fn);
        m_language = LangType::L_CPP;
    };
    auto get_file_name() const
    {
        return m_full_path;
    }
    void is_file_read_only()
    {
        struct _stat buf;
        if (!_stat(m_full_path, &buf))
        {
            m_is_read_only = (bool)(!(buf.st_mode & _S_IWRITE));
        }
    };
    bool is_dirty() const
    {
        return m_is_dirty;
    };
    bool is_read_only()
    {
        struct _stat buf;
        if (!_stat(m_full_path, &buf))
        {
            m_is_read_only = (bool)(!(buf.st_mode & _S_IWRITE));
        }
        return m_is_read_only;
    };

  private:
    // TODO: convert file path to wchar
    char* m_full_path{nullptr};
    bool m_is_dirty;
    Document m_doc;
    LangType m_language;
    bool m_is_read_only;
    Position m_pos;
};