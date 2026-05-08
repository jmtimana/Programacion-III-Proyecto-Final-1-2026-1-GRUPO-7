#pragma once

#include <vector>
#include <string>
#include <fstream>

using namespace std;

vector<string> parseCSVLine(const string& line);

vector<string> readCSVRows(ifstream& file);