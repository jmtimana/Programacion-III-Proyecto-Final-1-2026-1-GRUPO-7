#pragma once

#include "Movie.h"
#include "Trie.h"
#include <vector>

using namespace std;

class SearchEngine {

private:

    Trie trie;

    vector<Movie> movies;

public:

    void loadCSV(const string& filename);

    vector<int> search(const string& query);

    vector<int> rankResults(
        const vector<int>& ids,
        const string& query
        );

    Movie getMovie(int id);
};