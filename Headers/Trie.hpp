#pragma once
#include <string>
#define ALPHABET_SIZE (26)

// Converts key current character into index
// use only 'a' through 'z' and lower case
#define CHAR_TO_INDEX(c) ((int)c - (int)'a')

// Trie node
class TrieNode
{
    class TrieNode* children[ALPHABET_SIZE];

    // isWordEnd is true if the node represents end of a word
    bool isWordEnd;

  public:
    // Returns new trie node (initialized to NULLs)
    TrieNode* getNode(void);

    // If not present, inserts key into trie.
    // If the key is prefix of trie node, just marks leaf node
    void insert(class TrieNode* root, const std::string key);

    // Returns true if key presents in trie, else false
    bool search(class TrieNode* root, const std::string key);

    // Returns 0 if current node has a child
    // If all children are NULL, return 1.
    bool isLastNode(class TrieNode* root);

    // Recursive function to print auto-suggestions for given node.
    void suggestionsRec(class TrieNode* root, std::string currPrefix);

    // Print suggestions for given query prefix.
    // int printAutoSuggestions(TrieNode* root, const std::string query);
    std::string printAutoSuggestions(TrieNode* root, const std::string query);
};

void initialize(class TrieNode* root);
