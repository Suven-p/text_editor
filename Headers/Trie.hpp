#pragma once

#include <string>
#include <vector>
constexpr int ALPHABET_SIZE = 53;

class Trie
{
    Trie* children[ALPHABET_SIZE];
    bool is_word_end;

  public:
    Trie()
    {
        is_word_end = false;
        for (int i = 0; i < ALPHABET_SIZE; i++)
            children[i] = NULL;
    }
    Trie(const Trie&) = delete;
    Trie& operator=(const Trie&) = delete;
    void insert(const std::string key);
    bool search(const std::string key);
    bool is_last_node();
    std::vector<std::string> get_all_leaves(Trie* root, std::string prefix);
    std::string get_suggestion(const std::string to_search);
};

void initialize_trie(Trie* root);