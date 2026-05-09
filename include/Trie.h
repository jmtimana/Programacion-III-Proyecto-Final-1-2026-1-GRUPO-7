#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>

using namespace std;

struct Node {

    unordered_map<char, Node*> children;

    vector<int> movieIDs;
};

class Trie {

private:

    Node* root;
    void deleteNode(Node* node);

public:

    Trie();
    ~Trie();
    
    void insertSingleWord(int id, const string& word);

    void insertSuffixes(int id, const string& word);

    void insert(int id, string text);

    vector<int> search(string query);
};