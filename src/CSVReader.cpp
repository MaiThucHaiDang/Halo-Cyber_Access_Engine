#include "CSVReader.h"
#include <fstream>
#include <iostream>

extern unsigned long long fast_stoull(const char* s);

bool isValidLocation(const string& loc)
{
	string locs[] = { "US", "VN", "JP", "KR", "SG", "CN", "DE", "FR", "UK", "AU", "CA", "IN", "BR", "RU", "TH" };
	for (int i = 0; i < 15; ++i) 
	{
		if (loc == locs[i])
		{
			return true;
		}
	}
	return false;
}

int loadData(const string& filename, DataArray& datas)
{
	ifstream file(filename);
	if (!file.is_open())
	{
		cout << "Khong the mo file " << filename << endl;
		return 0;
	}

	file.seekg(0, ios::end);
	size_t fileSize = file.tellg();
	file.seekg(0, ios::beg);
	datas.fileData = new char[fileSize + 1];
	file.read(datas.fileData, fileSize);
	size_t actualSize = file.gcount(); // Kích thước thật sự của mảng bị giảm đi đúng bằng số lần xuống dòng trong file, do hàm read đã lọc đi \r mỗi lần xuống dòng
	datas.fileData[actualSize] = '\0';
	file.close();

	if (!actualSize)
	{
		return 0;
	}

	int rows = 0;
	int cols = 1;
	size_t i = 0;

	for (; i < actualSize; ++i)
	{
		if (datas.fileData[i] == ',')
		{
			++cols;
		}
		else if (datas.fileData[i] == '\n')
		{
			++rows;
			++i;
			break;
		}
	}

	for (; i < actualSize; ++i)
	{
		if (datas.fileData[i] == '\n')
		{
			++rows;
		}
	}

	// Xử lí dòng cuối nếu không xuống dòng
	if (datas.fileData[actualSize - 1] != '\n')
	{
		++rows;
	}

	datas.initColumns(cols);
	if (--rows <= 0)
	{
		return 0;
	}

	datas.initRows(rows);
	datas.fileDataArr = new char* [rows * cols];
	int currentCol = 0;
	char* currentWord = datas.fileData;

	for (i = 0; i < actualSize; ++i)
	{
		bool isComma = (datas.fileData[i] == ',');
		bool isNewline = (datas.fileData[i] == '\n');

		if (isComma || isNewline)
		{
			datas.fileData[i] = '\0';
			datas.setColumnName(currentCol, string(currentWord));
			++currentCol;
			currentWord = &datas.fileData[i + 1];

			if (isNewline)
			{
				++i;
				break;
			}
		}
	}

	int currentRow = 0;
	currentCol = 0;

	int locIdx = datas.getColumnIndex("location");
	int timeIdx = datas.getColumnIndex("timestamp");

	for (; i < actualSize; ++i)
	{
		bool isComma = (datas.fileData[i] == ',');
		bool isNewline = (datas.fileData[i] == '\n');

		if (isComma || isNewline)
		{
			datas.fileData[i] = '\0';
			if (currentCol < cols)
			{
				datas.fileDataArr[currentRow * cols + currentCol] = currentWord;
			}
			++currentCol;

			if (isNewline)
			{
				if (currentCol != cols ||
					locIdx != -1 && !isValidLocation(datas.fileDataArr[currentRow * cols + locIdx]) ||
					timeIdx != -1 && !fast_stoull(datas.fileDataArr[currentRow * cols + timeIdx]))
				{
					--datas.rowSize;
					currentCol = 0;
					currentWord = &datas.fileData[i + 1];
					continue;
				}

				datas.rows[currentRow] = &datas.fileDataArr[currentRow * cols];
				++currentRow;
				currentCol = 0;
			}

			currentWord = &datas.fileData[i + 1];
		}
	}

	if (currentRow < datas.rowSize)
	{
		if (currentCol < cols)
		{
			datas.fileDataArr[currentRow * cols + currentCol] = currentWord;
		}
		++currentCol;

		if (currentCol != cols ||
			locIdx != -1 && !isValidLocation(datas.fileDataArr[currentRow * cols + locIdx]) ||
			timeIdx != -1 && !fast_stoull(datas.fileDataArr[currentRow * cols + timeIdx]))
		{
			--datas.rowSize;
		}
		else
		{
			datas.rows[currentRow] = &datas.fileDataArr[currentRow * cols];
		}
	}

	return datas.rowSize;
}
