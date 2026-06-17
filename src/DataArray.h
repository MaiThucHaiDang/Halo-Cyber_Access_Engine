#pragma once
#include <string>
using namespace std;

struct DataArray
{
	char*** rows;
	char** fileDataArr;
	char* fileData;
	string* columnNames;
	int columnSize;
	int rowSize;

	DataArray() : rows(new char** [10]),
		fileDataArr(nullptr),
		fileData(nullptr),
		columnNames(nullptr),
		columnSize(0),
		rowSize(0) {
	}

	~DataArray()
	{
		delete[] rows;
		delete[] columnNames;
		delete[] fileData;
		delete[] fileDataArr;
	}

	void initColumns(int size)
	{
		if (columnNames)
		{
			delete[] columnNames;
		}
		columnSize = size;
		columnNames = new string[size];
	}

	void initRows(int size)
	{
		if (rows)
		{
			delete[] rows;
		}
		rowSize = size;
		rows = new char** [size];
	}

	void setColumnName(int index, const string& name)
	{
		if (index >= 0 && index < columnSize && columnNames)
		{
			columnNames[index] = name;
		}
	}

	int getColumnIndex(const string& name) const
	{
		if (!columnNames)
		{
			return -1;
		}

		for (int i = 0; i < columnSize; ++i)
		{
			if (columnNames[i] == name)
			{
				return i;
			}
		}

		return -1;
	}
};
