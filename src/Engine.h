#pragma once
#include <string>
#include "DataArray.h"
using namespace std;

struct NumIdx
{
	unsigned long long val;
	int idx;
};

struct StrIdx
{
	const char* val;
	int idx;
};

StrIdx* sortIdxArr(const DataArray& datas, const string& columnName);
unsigned long long fast_stoull(const char* s);
void sortByColumn(DataArray& logArray, const string& columnName, bool isNumeric);
int getLowerBound(StrIdx* arr, int size, const char* target);
int getUpperBound(StrIdx* arr, int size, const char* target);
StrIdx* sortIdxArr(const DataArray& datas, const string& columnName);
void getUserJourney(const DataArray& logArray, StrIdx* userIndex, const string& user_id, unsigned long long startTime, unsigned long long endTime);
void getResourceJourney(const DataArray& logArray, StrIdx* resourceIndex, const string& resource_id, unsigned long long startTime, unsigned long long endTime);
void getTop10Resources(const DataArray& logArray, StrIdx* resourceIdxArr, unsigned long long startTime, unsigned long long endTime);
