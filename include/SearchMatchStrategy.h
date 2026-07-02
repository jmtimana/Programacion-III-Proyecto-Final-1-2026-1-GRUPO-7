#pragma once
#include <string>
#include <vector>

class ISearchMatchStrategy {
public:
    virtual ~ISearchMatchStrategy() = default;

    virtual bool matches(
        const std::string& field,
        const std::vector<std::string>& queryWords,
        const std::string& fullQuery
    ) const = 0;

    virtual std::string nombre() const = 0;
};

class AllWordsMatchStrategy : public ISearchMatchStrategy {
public:
    bool matches(
        const std::string& field,
        const std::vector<std::string>& queryWords,
        const std::string& fullQuery
    ) const override;

    std::string nombre() const override { return "Todas las palabras (AND)"; }
};

class AnyWordMatchStrategy : public ISearchMatchStrategy {
public:
    bool matches(
        const std::string& field,
        const std::vector<std::string>& queryWords,
        const std::string& fullQuery
    ) const override;

    std::string nombre() const override { return "Cualquier palabra (OR)"; }
};

class ExactPhraseMatchStrategy : public ISearchMatchStrategy {
public:
    bool matches(
        const std::string& field,
        const std::vector<std::string>& queryWords,
        const std::string& fullQuery
    ) const override;

    std::string nombre() const override { return "Frase exacta"; }
};
