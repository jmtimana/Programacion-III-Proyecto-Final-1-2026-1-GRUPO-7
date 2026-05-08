#include "../include/SearchEngine.h"
#include "../include/Parser.h"
#include "../include/Utils.h"
#include <iostream>
#include <fstream>
#include <algorithm>

void SearchEngine::loadCSV(const string& filename) {

    ifstream file(filename);

    if (!file.is_open()) {

        cout << "Error opening CSV\n";
        return;
    }

    vector<string> rows = readCSVRows(file);
    movies.reserve(rows.size());//para reservar memoria
    //evita que se meuvabn los datos
    int id = 0;

    for (int i = 1; i < rows.size(); i++) {

        vector<string> row = parseCSVLine(rows[i]);

        if (row.size() != 8)
            continue;

        Movie movie;

        movie.id = id;

        movie.title = row[1];
        movie.cast = row[3];
        movie.director = row[4];
        movie.genre = row[5];
        movie.plot = row[7];

        movie.normalizedTitle = normalize(movie.title);
        movie.normalizedPlot = normalize(movie.plot);
        movie.normalizedGenre = normalize(movie.genre);
        movie.normalizedDirector = normalize(movie.director);
        movie.normalizedCast = normalize(movie.cast);

        movie.normalizedText =
            movie.title + " " +
            movie.cast + " " +
            movie.director + " " +
            movie.genre + " " +
            movie.plot;

        movies.push_back(movie);

        trie.insert(id, movie.normalizedText);

        id++;
    }

    cout << "Movies loaded: " << movies.size() << endl;
}

vector<int> SearchEngine::search(const string& query) {

    vector<int> result = trie.search(query);

    return vector<int>(result.begin(), result.end());
}

Movie SearchEngine::getMovie(int id) {

    return movies[id];
}

vector<int> SearchEngine::rankResults( const vector<int>& ids, const string& query) {

    vector<pair<int,int>> scored;
    vector<string> queryWords = tokenize(normalize(query));

    string fullQuery = normalize(query);

    for (int id : ids) {
        const Movie& movie = movies[id];
        int score = 0;

        const string& title = movie.normalizedTitle;
        const string& plot = movie.normalizedPlot;
        const string& genre = movie.normalizedGenre;
        const string& director = movie.normalizedDirector;
        const string& cast = movie.normalizedCast;

        for (const string& word : queryWords) {
            if (title.find(word) != string::npos)
                score += 10;
            if (plot.find(word) != string::npos)
                score += 3;
            if (genre.find(word) != string::npos)
                score += 2;
            if (director.find(word) != string::npos)
                score += 2;
            if (cast.find(word) != string::npos)
                score += 1;
        }

        if (title.find(fullQuery) != string::npos)
            score += 20;

        scored.push_back({score, id});
    }

    sort(
        scored.begin(),
        scored.end(),
        greater<pair<int,int>>()
    );

    vector<int> ranked;

    for (auto& p : scored)
        ranked.push_back(p.second);

    return ranked;
}