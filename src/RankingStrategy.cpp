#include "../include/RankingStrategy.h"
#include "../include/Utils.h"
#include <algorithm>

std::vector<int> RelevanceRankingStrategy::rank(
    const std::vector<int>& ids,
    const std::string& query,
    const std::vector<Movie>& movies
) {
    std::vector<std::pair<int,int>> scored;
    std::vector<std::string> queryWords = tokenize(normalize(query));
    std::string fullQuery = normalize(query);

    for (int id : ids) {
        const Movie& movie = movies[id];
        int score = 0;

        for (const std::string& word : queryWords) {
            if (movie.normalizedTitle.find(word) != std::string::npos)
                score += 10;
            if (movie.normalizedPlot.find(word) != std::string::npos)
                score += 3;
            if (movie.normalizedGenre.find(word) != std::string::npos)
                score += 2;
            if (movie.normalizedDirector.find(word) != std::string::npos)
                score += 2;
            if (movie.normalizedCast.find(word) != std::string::npos)
                score += 1;
        }

        if (movie.normalizedTitle.find(fullQuery) != std::string::npos)
            score += 20;

        scored.push_back({score, id});
    }

    std::sort(
        scored.begin(),
        scored.end(),
        std::greater<std::pair<int,int>>()
    );

    std::vector<int> ranked;
    ranked.reserve(scored.size());
    for (auto& p : scored)
        ranked.push_back(p.second);

    return ranked;
}

std::vector<int> AlphabeticalRankingStrategy::rank(
    const std::vector<int>& ids,
    const std::string& /*query*/,
    const std::vector<Movie>& movies
) {
    std::vector<int> ranked = ids;

    std::sort(
        ranked.begin(),
        ranked.end(),
        [&movies](int a, int b) {
            return movies[a].normalizedTitle < movies[b].normalizedTitle;
        }
    );

    return ranked;
}
