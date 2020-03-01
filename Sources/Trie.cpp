#include "Trie.hpp"
#include <iostream>
#include <string>
#include <vector>

#define ALPHABET_SIZE (26)
// Converts key current character into index
// use only 'a' through 'z' and lower case
#define CHAR_TO_INDEX(c) ((int)c - (int)'a')

// Returns new trie node (initialized to NULLs)
TrieNode* TrieNode ::getNode(void)
{
    class TrieNode* pNode = new TrieNode;
    pNode->isWordEnd = false;

    for (int i = 0; i < ALPHABET_SIZE; i++)
        pNode->children[i] = NULL;

    return pNode;
}

// If not present, inserts key into trie. If the
// key is prefix of trie node, just marks leaf node
void TrieNode ::insert(class TrieNode* root, const std::string key)
{
    class TrieNode* pCrawl = root;

    for (int level = 0; level < key.length(); level++)
    {
        int index = CHAR_TO_INDEX(key[level]);
        if (!pCrawl->children[index])
            pCrawl->children[index] = getNode();

        pCrawl = pCrawl->children[index];
    }

    // mark last node as leaf
    pCrawl->isWordEnd = true;
}

// Returns true if key presents in trie, else false
bool TrieNode ::search(class TrieNode* root, const std::string key)
{
    int length = key.length();
    class TrieNode* pCrawl = root;
    for (int level = 0; level < length; level++)
    {
        int index = CHAR_TO_INDEX(key[level]);

        if (!pCrawl->children[index])
            return false;

        pCrawl = pCrawl->children[index];
    }

    return (pCrawl != NULL && pCrawl->isWordEnd);
}

// Returns 0 if current node has a child
// If all children are NULL, return 1.
bool TrieNode ::isLastNode(class TrieNode* root)
{
    for (int i = 0; i < ALPHABET_SIZE; i++)
        if (root->children[i])
            return 0;
    return 1;
}

// Recursive function to print auto-suggestions for given
// node.
void TrieNode ::suggestionsRec(class TrieNode* root, std::string currPrefix)
{
    // found a string in Trie with the given prefix
    std::string list_to_return = {};
    if (root->isWordEnd)
    {
        list_to_return.append(currPrefix);
        // std::cout << currPrefix;
        // std::cout << std::endl;
    }

    // All children struct node pointers are NULL
    if (isLastNode(root))
        return;

    for (int i = 0; i < ALPHABET_SIZE; i++)
    {
        if (root->children[i])
        {
            // append current character to currPrefix string
            currPrefix.push_back(97 + i);

            // recur over the rest
            suggestionsRec(root->children[i], currPrefix);
            // remove last character
            currPrefix.pop_back();
        }
    }
}

// Print suggestions for given query prefix.
std::string TrieNode ::printAutoSuggestions(TrieNode* root, const std::string query)
{
    struct TrieNode* pCrawl = root;
    std::string list_to_return = {};

    // Check if prefix is present and find the
    // the node (of last level) with last character of given string.
    int level;
    int n = query.length();
    for (level = 0; level < n; level++)
    {
        int index = CHAR_TO_INDEX(query[level]);

        // no string in the Trie has this prefix
        if (!pCrawl->children[index])
            return 0;

        pCrawl = pCrawl->children[index];
    }

    // If prefix is present as a word.
    bool isWord = (pCrawl->isWordEnd == true);

    // If prefix is last node of tree (has no children)
    bool isLast = isLastNode(pCrawl);

    // If prefix is present as a word, but there is no subtree below the last
    // matching node.
    if (isWord && isLast)
    {
        list_to_return.append(query);
        // std::cout << query << std::endl;
        // return -1;
        return list_to_return;
    }

    // If there are are nodes below last
    // matching character.
    if (!isLast)
    {
        std::string prefix = query;
        suggestionsRec(pCrawl, prefix);
        return 1;
    }
}

// Initializes trie with C++ standard keywords
void initialize(class TrieNode* root)
{
    std::vector<std::string> keywords = {"else",
                                         "new",
                                         "this"
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
        root->insert(root, temp);
    }
}

/*
// print suggestions for given query prefix.
int printAutoSuggestions(TrieNode* root, const string query)
{
    struct TrieNode* pCrawl = root;

    // Check if prefix is present and find the
    // the node (of last level) with last character
    // of given string.
    int level;
    int n = query.length();
    for (level = 0; level < n; level++)
    {
        int index = CHAR_TO_INDEX(query[level]);

        // no string in the Trie has this prefix
        if (!pCrawl->children[index])
            return 0;

        pCrawl = pCrawl->children[index];
    }

    // If prefix is present as a word.
    bool isWord = (pCrawl->isWordEnd == true);

    // If prefix is last node of tree (has no
    // children)
    bool isLast = isLastNode(pCrawl);

    // If prefix is present as a word, but
    // there is no subtree below the last
    // matching node.
    if (isWord && isLast)
    {
        cout << query << endl;
        return -1;
    }

    // If there are are nodes below last
    // matching character.
    if (!isLast)
    {
        string prefix = query;
        suggestionsRec(pCrawl, prefix);
        return 1;
    }
}

*/