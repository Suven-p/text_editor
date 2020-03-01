#pragma once

#include "Scintilla.h"
#include <shlwapi.h>
#include <stdexcept>
#include <sys/stat.h>
#include <sys/types.h>

constexpr wchar_t DEFAULT_TAB_NAME[] = L"New ";
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
    Buffer(Document doc, std::wstring fileName) : m_doc(doc), m_is_dirty(false), m_is_read_only(false)
    {
        m_full_path = new wchar_t[MAX_PATH];
        m_pos.first_line = 0;
        m_pos.start_pos = 0;
        m_pos.end_pos = 0;
        set_file_name(fileName);
    }
    Buffer(const Buffer& buf)
        : m_is_dirty(buf.m_is_dirty), m_doc(buf.m_doc), m_language(buf.m_language), m_is_read_only(buf.m_is_read_only),
          m_pos(buf.m_pos)
    {
        m_full_path = new wchar_t[MAX_PATH];
        wcscpy(m_full_path, buf.m_full_path);
    }
    Buffer& operator=(const Buffer& buf) noexcept
    {
        if (this != &buf)
        {
            m_is_dirty = buf.m_is_dirty;
            m_doc = buf.m_doc;
            m_language = buf.m_language;
            m_is_read_only = buf.m_is_read_only;
            m_pos = buf.m_pos;
            wcscpy(m_full_path, buf.m_full_path);
        }
        return *this;
    }
    Buffer(Buffer&& buf) noexcept
        : m_is_dirty(std::move(buf.m_is_dirty)), m_doc(std::move(buf.m_doc)),
          m_is_read_only(std::move(buf.m_is_read_only)), m_pos(std::move(buf.m_pos))
    {
        m_language = buf.m_language;
        m_full_path = buf.m_full_path;
        buf.m_full_path = nullptr;
    }
    Buffer& operator=(Buffer&& buf) noexcept
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
    // TODO: Add support for non cpp extension
    void set_file_name(const std::wstring file_name)
    {
        wcscpy(m_full_path, file_name.c_str());
        m_language = LangType::L_CPP;
    }
    auto get_file_name() const
    {
        return m_full_path;
    }
    bool is_file_read_only()
    {
        DWORD attributes = GetFileAttributesW(m_full_path);
        if (attributes != INVALID_FILE_ATTRIBUTES)
        {
            return (attributes & FILE_ATTRIBUTE_READONLY);
        }
        else
        {
            std::string err_msg = "Error " + std::to_string(GetLastError()) + " Cannot create scintilla window";
            throw std::runtime_error(err_msg);
        }
    }
    bool is_read_only()
    {
        if (is_file_read_only()) {
            m_is_read_only = true;
        }
        return m_is_read_only;
    }
    bool is_dirty() const
    {
        return m_is_dirty;
    }

  private:
    // TODO: convert file path to wchar
    wchar_t* m_full_path{nullptr};
    bool m_is_dirty;
    Document m_doc;
    LangType m_language;
    bool m_is_read_only;
    Position m_pos;
};