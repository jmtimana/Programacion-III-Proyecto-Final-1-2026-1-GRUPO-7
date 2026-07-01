#pragma once
#include <vector>
#include <string>
#include "Movie.h"

class IRankingStrategy {
public:
    virtual ~IRankingStrategy() = default;

    virtual std::vector<int> rank(
        const std::vector<int>& ids,
        const std::string& query,
        const std::vector<Movie>& movies
    ) = 0;

    virtual std::string nombre() const = 0;
};

class RelevanceRankingStrategy : public IRankingStrategy {
public:
    std::vector<int> rank(
        const std::vector<int>& ids,
        const std::string& query,
        const std::vector<Movie>& movies
    ) override;

    std::string nombre() const override { return "Relevancia"; }
};

class AlphabeticalRankingStrategy : public IRankingStrategy {
public:
    std::vector<int> rank(
        const std::vector<int>& ids,
        const std::string& query,
        const std::vector<Movie>& movies
    ) override;

    std::string nombre() const override { return "Alfabetico (A-Z)"; }
};
