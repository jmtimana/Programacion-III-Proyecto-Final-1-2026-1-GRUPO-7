#include "../include/Trie.h"
#include "../include/Utils.h"
#include <algorithm>

Trie::Trie() {

    root = new Node();
}


void Trie::insertSingleWord(int id, const string& word) {
    Node* node = root;
    for (char c : word) {
        if (!node->children.count(c))
            node->children[c] = new Node();
        node = node->children[c];

        //evita duplicados
        if (node->movieIDs.empty() || node->movieIDs.back() != id) {
            node->movieIDs.push_back(id);
        }
    }
}


void Trie::insertSuffixes(
    int id,
    const string& word
) {

    int limit = min((int)word.size(), 6);

    for (int i = 0; i < limit; i++) {

        string suffix = word.substr(i);

        if (suffix.size() < 3)
            continue;

        insertSingleWord(id, suffix);
    }
}

void Trie::insert(int id, string text) {

    text = normalize(text);

    vector<string> words = tokenize(text);

    unordered_set<string> uniqueWords(
    words.begin(),
    words.end()
    );

    for (const string& word : uniqueWords) {

        if (word.size() >= 3)
            insertSuffixes(id, word);
    }
}

vector<int> Trie::search(string query) {

    query = normalize(query);

    vector<string> words = tokenize(query);

    vector<int> allResults;
    allResults.reserve(2000);  //para evitar realocaciones, ajusta si necesitas mas

    for (const string& word : words) {

        if (word.size() < 3)
            continue;

        Node* node = root;

        bool existe = true;

        for (char c : word) {

            auto it = node->children.find(c);

            if (it == node->children.end()) {

                existe = false;
                break;
            }

            node = it->second;
        }

        if (!existe)
            continue;

        allResults.insert(
            allResults.end(),
            node->movieIDs.begin(),
            node->movieIDs.end()
        );
    }

    sort(allResults.begin(), allResults.end());
    allResults.erase(
        unique(allResults.begin(), allResults.end()),
        allResults.end()
    );

    return allResults;
}