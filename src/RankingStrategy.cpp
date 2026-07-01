#include "../include/RankingStrategy.h"
#include "../include/Utils.h"
#include <algorithm>

constexpr int SCORE_TITLE_EXACT = 20;
constexpr int SCORE_TITLE_WORD = 10;
constexpr int SCORE_PLOT_WORD = 3;
constexpr int SCORE_GENRE_WORD = 2;
constexpr int SCORE_DIRECTOR_WORD = 2;
constexpr int SCORE_CAST_WORD = 1;

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
                score += SCORE_TITLE_WORD;
            if (movie.normalizedPlot.find(word) != std::string::npos)
                score += SCORE_PLOT_WORD;
            if (movie.normalizedGenre.find(word) != std::string::npos)
                score += SCORE_GENRE_WORD;
            if (movie.normalizedDirector.find(word) != std::string::npos)
                score += SCORE_DIRECTOR_WORD;
            if (movie.normalizedCast.find(word) != std::string::npos)
                score += SCORE_CAST_WORD;
        }

        if (movie.normalizedTitle.find(fullQuery) != std::string::npos)
            score += SCORE_TITLE_EXACT;

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
