#pragma once

#include <string>
#include <vector>
#include <unordered_set>

using namespace std;

string normalize(string s);

string normalizeAccents(string s);

vector<string> tokenize(const string& text);