#include "../include/Parser.h"

vector<string> parseCSVLine(const string& line) {

    vector<string> resultado;

    string current;

    bool caso = false;

    for (char c : line) {

        if (c == '"')
            caso = !caso;

        else if (c == ',' && !caso) {

            resultado.push_back(current);
            current.clear();

        } else {

            current += c;
        }
    }

    resultado.push_back(current);

    return resultado;
}

vector<string> readCSVRows(ifstream& file) {

    vector<string> rows;

    string line, current;

    bool inQuotes = false;

    while (getline(file, line)) {

        if (!current.empty())
            current += "\n";

        current += line;

        for (int i = 0; i < line.size(); i++) {

            if (line[i] == '"') {

                if (i + 1 < line.size() && line[i + 1] == '"')
                    i++;

                else
                    inQuotes = !inQuotes;
            }
        }

        if (!inQuotes) {

            rows.push_back(current);
            current.clear();
        }
    }

    return rows;
}