#include "Trie.hpp"

int char_to_index(char c)
{
    if (isalpha(c))
    {
        if (islower(c))
        {
            return static_cast<int>(c) - static_cast<int>('a');
        }
        if (isupper(c))
        {
            return static_cast<int>(c) - static_cast<int>('A') + 26;
        }
    }
    else if (c == '_')
        return 52;
    return -1;
}

char index_to_char(int i)
{
    if (i >= 0 && i <= 25)
        return static_cast<char>(i + 'a');
    else if (i > 25 && i <= 51)
        return static_cast<char>((i - 26) + 'A');
    else if (i == 52)
        return '_';
    else
        return 0;
}

void Trie::insert(const std::string key)
{
    Trie* cur_ptr = this;
    for (int level = 0; level < key.length(); level++)
    {
        int index = char_to_index(key[level]);
        if (!cur_ptr->children[index])
            cur_ptr->children[index] = new Trie;
        cur_ptr = cur_ptr->children[index];
    }
    cur_ptr->is_word_end = true;
}

bool Trie::search(const std::string key)
{
    int length = key.length();
    Trie* cur_ptr = this;
    for (int level = 0; level < length; level++)
    {
        int index = char_to_index(key[level]);
        if (!cur_ptr->children[index])
            return false;
        cur_ptr = cur_ptr->children[index];
    }
    return (cur_ptr != NULL && cur_ptr->is_word_end);
}

bool Trie::is_last_node()
{
    Trie* cur_ptr = this;
    for (int i = 0; i < ALPHABET_SIZE; i++)
        if (cur_ptr->children[i])
            return false;
    return true;
}

std::vector<std::string> Trie::get_all_leaves(Trie* root, std::string prefix)
{
    std::vector<std::string> leaves;
    std::string cur_prefix = prefix;
    // found a string in Trie with the given prefix
    if (root->is_word_end)
    {
        leaves.push_back(prefix);
    }
    if (root->is_last_node())
        return leaves;
    for (int i = 0; i < ALPHABET_SIZE; i++)
    {
        if (root->children[i])
        {
            cur_prefix.push_back(index_to_char(i));
            for (auto val : get_all_leaves(root->children[i], cur_prefix))
            {
                leaves.push_back(val);
            }
            cur_prefix.pop_back();
        }
    }
    return leaves;
}

std::string Trie ::get_suggestion(const std::string to_search)
{
    struct Trie* current_ptr = this;
    std::string list_to_return = {};
    for (size_t level = 0, len = to_search.length(); level < len; level++)
    {
        int index = char_to_index(to_search[level]);
        if (!current_ptr->children[index])
            return list_to_return;
        current_ptr = current_ptr->children[index];
    }
    if (current_ptr->is_word_end)
    {
        list_to_return += std::string(to_search + ' ');
    }
    if (current_ptr->is_last_node())
    {
        return list_to_return;
    }
    for (auto val : get_all_leaves(current_ptr, to_search))
    {
        list_to_return += std::string(val + ' ');
    }
    list_to_return.pop_back();
    return list_to_return;
}

// Initialize trie with C++ standard keywords
void initialize_trie(Trie* root)
{
    std::vector<std::string> keywords = {"else",
                                         "new",
                                         "this",
                                         "auto",
                                         "enum",
                                         "operator",
                                         "throw",
                                         "bool",
                                         "explicit",
                                         "private",
                                         "true",
                                         "break",
                                         "export",
                                         "protected",
                                         "try",
                                         "case",
                                         "extern",
                                         "public",
                                         "typedef",
                                         "catch",
                                         "false",
                                         "register",
                                         "typeid",
                                         "char",
                                         "float",
                                         "reinterpret_cast",
                                         "typename",
                                         "class",
                                         "union",
                                         "const",
                                         "friend",
                                         "short",
                                         "unsigned",
                                         "goto",
                                         "signed",
                                         "using"
                                         "continue",
                                         "sizeof",
                                         "virtual",
                                         "default",
                                         "inline",
                                         "static",
                                         "void",
                                         "delete",
                                         "int",
                                         "volatile",
                                         "long",
                                         "struct",
                                         "double",
                                         "mutable",
                                         "switch",
                                         "while",
                                         "namespace",
                                         "template"};
    for (auto temp : keywords)
    {
        root->insert(temp);
    }
}

/*
// print suggestions for given to_search prefix.
int get_suggestion(Trie* root, const string to_search)
{
    struct Trie* current_ptr = root;

    // Check if prefix is present and find the
    // the node (of last level) with last character
    // of given string.
    int level;
    int len = to_search.length();
    for (level = 0; level < len; level++)
    {
        int index = char_to_index(to_search[level]);

        // no string in the Trie has this prefix
        if (!current_ptr->children[index])
            return 0;

        current_ptr = current_ptr->children[index];
    }

    // If prefix is present as a word.
    bool isWord = (current_ptr->is_word_end == true);

    // If prefix is last node of tree (has no
    // children)
    bool isLast = is_last_node(current_ptr);

    // If prefix is present as a word, but
    // there is no subtree below the last
    // matching node.
    if (isWord && isLast)
    {
        cout << to_search << endl;
        return -1;
    }

    // If there are are nodes below last
    // matching character.
    if (!isLast)
    {
        string prefix = to_search;
        get_all_leaves(current_ptr, prefix);
        return 1;
    }
}
*/