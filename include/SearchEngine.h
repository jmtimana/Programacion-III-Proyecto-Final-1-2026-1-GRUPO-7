#pragma once

#include "Movie.h"
#include "Trie.h"
#include "RankingStrategy.h"
#include "SearchMatchStrategy.h"
#include <vector>
#include <functional>
#include <atomic>
using namespace std;

class SearchEngine {

private:
    Trie trie;

    vector<Movie> movies;

    IRankingStrategy* rankingStrategy = new RelevanceRankingStrategy();

    ISearchMatchStrategy* matchStrategy = new AllWordsMatchStrategy();

public:

    ~SearchEngine() {
        delete rankingStrategy;
        delete matchStrategy;
    }

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
    int movieCount() const { return movies.size(); }

    void setRankingStrategy(IRankingStrategy* strategy) {
        delete rankingStrategy;
        rankingStrategy = strategy;
    }
    string rankingStrategyNombre() const {
        return rankingStrategy->nombre();
    }

    void setMatchStrategy(ISearchMatchStrategy* strategy) {
        delete matchStrategy;
        matchStrategy = strategy;
    }
    string matchStrategyNombre() const {
        return matchStrategy->nombre();
    }
};
