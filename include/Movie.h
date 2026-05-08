#pragma once
#include <string>

struct Movie {
    int id;
    std::string title;
    std::string genre;
    std::string cast;
    std::string director;
    std::string plot;
    std::string normalizedText;

    std::string normalizedTitle;
    std::string normalizedPlot;
    std::string normalizedGenre;
    std::string normalizedDirector;
    std::string normalizedCast;
};
