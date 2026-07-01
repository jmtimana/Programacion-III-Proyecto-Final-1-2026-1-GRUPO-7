#pragma once

#include "Movie.h"
#include "Trie.h"
#include "RankingStrategy.h"
#include "SearchMatchStrategy.h"
#include <vector>
#include <functional>
#include <atomic>
#include <stdexcept>

using namespace std;

using FieldGetter = function<string(const Movie&)>;
using Resultados = vector<int>;

class SearchEngine {

private:
    // Constructor privado — nadie puede crear una instancia directamente
    SearchEngine() = default;

    Trie trie;

    vector<Movie> movies;

    IRankingStrategy* rankingStrategy = new RelevanceRankingStrategy();

    ISearchMatchStrategy* matchStrategy = new AllWordsMatchStrategy();

public:

    // Singleton: punto de acceso global
    static SearchEngine& getInstance() {
        static SearchEngine instance;
        return instance;
    }

    // Deshabilitar copia y asignación
    SearchEngine(const SearchEngine&)            = delete;
    SearchEngine& operator=(const SearchEngine&) = delete;

    ~SearchEngine() {
        delete rankingStrategy;
        delete matchStrategy;
    }

    void loadCSV(const string& filename);

    Resultados search(const string& query);

    Resultados searchByField(
        const string& query,
        FieldGetter fieldGetter
    );

    Resultados rankResults(
        const Resultados& ids,
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
