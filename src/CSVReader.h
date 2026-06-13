#pragma once
#include <string>
#include "DataArray.h"
using namespace std;

bool isValidLocation(const string& loc);
int loadData(const string& filename, DataArray& logArray);
