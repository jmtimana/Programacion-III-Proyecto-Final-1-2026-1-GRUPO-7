#include "../include/SearchEngine.h"
#include "../include/Parser.h"
#include "../include/Utils.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <thread>
#include <vector>
#include <chrono>
#include <stdexcept>

void SearchEngine::processChunk(
    const vector<string>& rows,
    int begin,
    int end)
{
    for (int i = begin; i < end; i++)
    {
        vector<string> row = parseCSVLine(rows[i]);

        // si la fila no tiene exactamente 8 columnas, está corrupta, la ignoramos
        if (row.size() != 8)
            continue;

        Movie movie;

        movie.id       = i - 1;
        movie.title    = row[1];
        movie.director = row[3];
        movie.cast     = row[4];
        movie.genre    = row[5];
        movie.plot     = row[7];

        // normalizar cada campo por separado para búsquedas más precisas
        movie.normalizedTitle    = normalize(movie.title);
        movie.normalizedPlot     = normalize(movie.plot);
        movie.normalizedGenre    = normalize(movie.genre);
        movie.normalizedDirector = normalize(movie.director);
        movie.normalizedCast     = normalize(movie.cast);

        // reservar memoria antes de concatenar para evitar realocaciones
        movie.normalizedText.reserve(
            movie.normalizedTitle.size()    +
            movie.normalizedCast.size()     +
            movie.normalizedDirector.size() +
            movie.normalizedGenre.size()    +
            movie.normalizedPlot.size()     +
            10
        );

        // superstring: une todos los campos para indexar en el Trie
        movie.normalizedText =
            movie.normalizedTitle    + " " +
            movie.normalizedCast     + " " +
            movie.normalizedDirector + " " +
            movie.normalizedGenre    + " " +
            movie.normalizedPlot;

        movies[movie.id] = std::move(movie);
    }
}

void SearchEngine::loadCSV(const string& filename)
{
    ifstream file(filename);

    // error: no se pudo abrir el archivo
    if (!file.is_open())
        throw runtime_error("No se pudo abrir el archivo: " + filename);

    cout << "Leyendo CSV...\n";

    auto startRead = chrono::high_resolution_clock::now();

    vector<string> rows = readCSVRows(file);

    auto endRead = chrono::high_resolution_clock::now();
    chrono::duration<double> readTime = endRead - startRead;
    cout << "Tiempo lectura CSV: " << readTime.count() << " segundos\n";

    // error: el CSV está vacío o solo tiene encabezado
    if (rows.size() <= 1)
        throw runtime_error("El archivo CSV esta vacio o no tiene datos: " + filename);

    movies.clear();
    movies.resize(rows.size() - 1);

    unsigned h = thread::hardware_concurrency();
    if (h == 0)
        h = 8;

    cout << "Threads utilizados: " << h << endl;

    int totalRows = static_cast<int>(rows.size()) - 1;

    vector<thread> threads;

    auto startParse = chrono::high_resolution_clock::now();

    for (unsigned t = 0; t < h; t++)
    {
        int begin = 1 + t * totalRows / h;
        int end   = 1 + (t + 1) * totalRows / h;

        threads.emplace_back(
            &SearchEngine::processChunk,
            this,
            cref(rows),
            begin,
            end
        );
    }

    for (auto& th : threads)
        th.join();

    auto endParse = chrono::high_resolution_clock::now();
    chrono::duration<double> parseTime = endParse - startParse;
    cout << "Tiempo parse/normalize: " << parseTime.count() << " segundos\n";

    cout << "Construyendo Trie...\n";

    auto startTrie = chrono::high_resolution_clock::now();

    for (const Movie& movie : movies)
        trie.insert(movie.id, movie.normalizedText);

    auto endTrie = chrono::high_resolution_clock::now();
    chrono::duration<double> trieTime = endTrie - startTrie;
    cout << "Tiempo Trie: " << trieTime.count() << " segundos\n";

    cout << "\nMovies loaded: " << movies.size() << endl;
}

Resultados SearchEngine::search(const string& query)
{
    // error: query vacío no tiene sentido buscar
    if (query.empty())
        throw invalid_argument("El query de busqueda no puede estar vacio");

    vector<int> result = trie.search(query);
    return Resultados(result.begin(), result.end());
}

Resultados SearchEngine::searchByField(
    const string& query,
    FieldGetter fieldGetter)
{
    // error: query vacío
    if (query.empty())
        throw invalid_argument("El query de busqueda no puede estar vacio");

    vector<int> candidates = trie.search(query);

    vector<string> queryWords = tokenize(normalize(query));
    string fullQuery = normalize(query);

    Resultados filtered;
    filtered.reserve(candidates.size());

    for (int id : candidates) {
        const string& field = fieldGetter(movies[id]);
        if (matchStrategy->matches(field, queryWords, fullQuery))
            filtered.push_back(id);
    }

    return rankResults(filtered, query);
}

Movie SearchEngine::getMovie(int id)
{
    // error: ID fuera del rango del vector de películas
    if (id < 0 || id >= (int)movies.size())
        throw out_of_range("ID de pelicula invalido: " + to_string(id));

    return movies[id];
}

Resultados SearchEngine::rankResults(const Resultados& ids, const string& query)
{
    return rankingStrategy->rank(ids, query, movies);
}
