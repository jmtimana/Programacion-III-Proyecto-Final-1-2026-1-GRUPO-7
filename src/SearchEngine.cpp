#include "../include/SearchEngine.h"
#include "../include/Parser.h"
#include "../include/Utils.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <thread>
#include <vector>
#include <chrono>
void SearchEngine::processChunk(
    const vector<string>& rows,
    int begin,
    int end)
{
    for(int i = begin; i < end; i++)
    {
        vector<string> row = parseCSVLine(rows[i]);

        if(row.size() != 8)
            continue;

        Movie movie;

        movie.id = i - 1;

        movie.title = row[1];
        movie.cast = row[3];
        movie.director = row[4];
        movie.genre = row[5];
        movie.plot = row[7];

        movie.normalizedTitle =
            normalize(movie.title);

        movie.normalizedPlot =
            normalize(movie.plot);

        movie.normalizedGenre =
            normalize(movie.genre);

        movie.normalizedDirector =
            normalize(movie.director);

        movie.normalizedCast =
            normalize(movie.cast);

        movie.normalizedText.reserve(
            movie.normalizedTitle.size() +
            movie.normalizedCast.size() +
            movie.normalizedDirector.size() +
            movie.normalizedGenre.size() +
            movie.normalizedPlot.size() +
            10
        );

        movie.normalizedText =
            movie.normalizedTitle + " " +
            movie.normalizedCast + " " +
            movie.normalizedDirector + " " +
            movie.normalizedGenre + " " +
            movie.normalizedPlot;

        movies[movie.id] = std::move(movie);
    }
}

void SearchEngine::loadCSV(const string& filename)
{
    ifstream file(filename);

    if(!file.is_open())
    {
        cout << "Error opening CSV\n";
        return;
    }

    cout << "Leyendo CSV...\n";

    auto startRead =
        chrono::high_resolution_clock::now();

    vector<string> rows =
        readCSVRows(file);

    auto endRead =
        chrono::high_resolution_clock::now();

    chrono::duration<double> readTime =
        endRead - startRead;

    cout << "Tiempo lectura CSV: "
         << readTime.count()
         << " segundos\n";

    if(rows.size() <= 1)
    {
        cout << "CSV vacio\n";
        return;
    }

    movies.clear();

    movies.resize(rows.size() - 1);

    unsigned h =
        thread::hardware_concurrency();

    if(h == 0)
        h = 8;

    cout << "Threads utilizados: "
         << h
         << endl;

    int totalRows =
        static_cast<int>(rows.size()) - 1;

    vector<thread> threads;

    auto startParse =
        chrono::high_resolution_clock::now();

    for(unsigned t = 0; t < h; t++)
    {
        int begin =
            1 + t * totalRows / h;

        int end =
            1 + (t + 1) * totalRows / h;

        threads.emplace_back(
            &SearchEngine::processChunk,
            this,
            cref(rows),
            begin,
            end
        );
    }

    for(auto& th : threads)
        th.join();

    auto endParse =
        chrono::high_resolution_clock::now();

    chrono::duration<double> parseTime =
        endParse - startParse;

    cout << "Tiempo parse/normalize: "
         << parseTime.count()
         << " segundos\n";

    cout << "Construyendo Trie...\n";

    auto startTrie =
        chrono::high_resolution_clock::now();

    for(const Movie& movie : movies)
    {
        trie.insert(
            movie.id,
            movie.normalizedText
        );
    }

    auto endTrie =
        chrono::high_resolution_clock::now();

    chrono::duration<double> trieTime =
        endTrie - startTrie;

    cout << "Tiempo Trie: "
         << trieTime.count()
         << " segundos\n";

    cout << "\nMovies loaded: "
         << movies.size()
         << endl;
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