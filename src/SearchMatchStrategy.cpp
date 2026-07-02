#include "../include/SearchMatchStrategy.h"

bool AllWordsMatchStrategy::matches(
    const std::string& field,
    const std::vector<std::string>& queryWords,
    const std::string& /*fullQuery*/
) const {
    if (queryWords.empty()) return false;

    for (const std::string& word : queryWords) {
        if (field.find(word) == std::string::npos) {
            return false;
        }
    }
    return true;
}

bool AnyWordMatchStrategy::matches(
    const std::string& field,
    const std::vector<std::string>& queryWords,
    const std::string& /*fullQuery*/
) const {
    for (const std::string& word : queryWords) {
        if (field.find(word) != std::string::npos) {
            return true;
        }
    }
    return false;
}

bool ExactPhraseMatchStrategy::matches(
    const std::string& field,
    const std::vector<std::string>& /*queryWords*/,
    const std::string& fullQuery
) const {
    if (fullQuery.empty()) return false;
    return field.find(fullQuery) != std::string::npos;
}
