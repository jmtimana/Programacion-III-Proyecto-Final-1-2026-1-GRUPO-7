#pragma once

#include "Movie.h"
#include "Trie.h"
#include <vector>
#include <functional>
#include <atomic>
using namespace std;

class SearchEngine {

private:
    Trie trie;

    vector<Movie> movies;
public:

    void loadCSV(const string& filename);

    vector<int> search(const string& query);

    vector<int> searchByField(
        const string& query,
        function<string(const Movie&)> fieldGetter
    );

    vector<int> rankResults(
        const vector<int>& ids,
        const string& query
        );
    void processChunk(
    const vector<string>& rows,
    int begin,
    int end
);
    Movie getMovie(int id);
};