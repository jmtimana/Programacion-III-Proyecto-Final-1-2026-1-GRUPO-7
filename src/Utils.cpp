#include "../include/Utils.h"
#include <algorithm>
#include <sstream>

string normalizeAccents(string s) {

    vector<vector<string>> groups = {

        {"á", "à", "ä","ä", "â", "ã", "å", "Á", "À", "Ä", "Â", "Ã", "Å","a"},
        {"ó", "ò", "ö", "ô", "õ", "ø","Ó", "Ò", "Ö", "Ô", "Õ", "Ø","o"},
        {"é", "è", "ë", "ê", "é","É", "È", "Ë", "Ê","e"},
        {"í", "ı","İ","ì", "ï", "î","Í", "Ì", "Ï", "Î","i"},
        {"ú", "ù", "ü","Ü","ü", "û","Ú", "Ù", "Ü", "Û","u"},
        {"ş","Ş","s"},
        {"ğ","Ğ","g"},
        {"[1]","[19]","[18]","[17]","[Note 1]", "[4]", "[2]","[3]","[13]","[6]", "[10]", "[11]", "[12]", ""},
        {"ç", "Ç","c"},
        {"ñ", "Ñ","n"},
        {"ý", "ÿ","Ý","y"},
        {"š", "Š","s"},
        {"ž", "Ž","z"},
        {"đ", "Đ","d"}
    };

    for (const auto& group : groups) {

        string replacement = group.back();

        for (int i = 0; i < group.size() - 1; i++) {

            size_t pos = 0;

            while (
                (pos = s.find(group[i], pos))
                != string::npos
            ) {

                s.replace(
                    pos,
                    group[i].length(),
                    replacement
                );

                pos += replacement.length();
            }
        }
    }

    return s;
}

string normalize(string s) {
    s = normalizeAccents(s);

    transform(s.begin(), s.end(), s.begin(), ::tolower);

    string result;

    for (unsigned char c : s) {
        if (isalnum(c) || c == ' ') {
            result += c;
        }
    }

    string clean;
    bool espacio = false;

    for (char c : result) {
        if (c == ' ') {
            if (!espacio) {
                clean += c;
                espacio = true;
            }
        } else {
            clean += c;
            espacio = false;
        }
    }

    return clean;
}

unordered_set<string> stopwords = {
    // Artículos y determinantes
    "the", "a", "an", "this", "that", "these", "those",

    // Preposiciones (las más comunes)
    "of", "in", "to", "for", "with", "on", "at", "from", "by",
    "about", "into", "through", "during", "before", "after",
    "above", "below", "up", "out", "off", "over", "under",
    "again", "further", "then", "once", "here", "there",

    // Conjunciones
    "and", "but", "or", "yet", "so", "if", "because", "although",
    "though", "while", "where", "when", "than", "then",

    // Pronombres personales y posesivos
    "i", "you", "he", "she", "it", "we", "they", "me", "him",
    "her", "us", "them", "my", "your", "his", "its", "our",
    "their", "mine", "yours", "hers", "ours", "theirs",

    // Pronombres relativos
    "who", "whom", "whose", "which", "what",

    // Verbos auxiliares y "to be"
    "is", "am", "are", "was", "were", "be", "been", "being",
    "have", "has", "had", "do", "does", "did",
    "will", "would", "shall", "should", "may", "might",
    "can", "could", "must",

    // Adverbios comunes sin significado propio
    "not", "no", "so", "just", "only", "also", "too",
    "very", "really", "still", "already", "yet", "even",
    "now", "here", "there", "everywhere", "somewhere",

    // Metadatos del dataset (ruido)
    "en", "org", "https", "http", "wikipedia", "wiki",
    "ref", "cite", "url", "accessed", "retrieved",

    // Números ordinales/cardinales genéricos
    "one", "two", "first", "second"
};

vector<string> tokenize(const string& text) {
    vector<string> words;
    stringstream ss(text);
    string word;

    while (ss >> word) {
        if (word.size() < 3) continue;
        if (stopwords.count(word)) continue;
        words.push_back(word);
    }
    return words;
}