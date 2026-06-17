#include "Engine.h"
#include <iostream>
#include <algorithm>
#include <cstring>
using namespace std;

StrIdx* sortIdxArr(const DataArray& datas, const string& columnName)
{
	int colIdx = datas.getColumnIndex(columnName);
	if (colIdx == -1) return nullptr;

	StrIdx* idxArr = new StrIdx[datas.rowSize];
	for (int i = 0; i < datas.rowSize; ++i)
	{
		idxArr[i].val = datas.rows[i][colIdx];
		idxArr[i].idx = i;
	}

	stable_sort(idxArr, idxArr + datas.rowSize, [](const StrIdx& a, const StrIdx& b)
		{
			return strcmp(a.val, b.val) < 0;
		});

	return idxArr;
}

unsigned long long fast_stoull(const char* s)
{
	if (!s || s[0] == '\0')
	{
		return 0;
	}

	while (*s == ' ')
	{
		++s;
	}

	unsigned long long res = 0;
	while (*s >= '0' && *s <= '9')
	{
		res = res * 10 + *s - '0';
		s++;
	}

	return res;
}

void sortByColumn(DataArray& datas, const string& columnName, bool isNum)
{
	int colIdx = datas.getColumnIndex(columnName);
	if (colIdx == -1)
	{
		cout << "Khong tim thay cot '" << columnName << "' de sap xep.\n";
		return;
	}

	if (isNum)
	{
		NumIdx* idxArr = new NumIdx[datas.rowSize];
		for (int i = 0; i < datas.rowSize; ++i)
		{
			idxArr[i].val = fast_stoull(datas.rows[i][colIdx]);
			idxArr[i].idx = i;
		}
		sort(idxArr, idxArr + datas.rowSize, [](const NumIdx& a, const NumIdx& b)
			{
				return a.val < b.val;
			});

		char*** newRows = new char** [datas.rowSize];
		for (int i = 0; i < datas.rowSize; ++i)
		{
			newRows[i] = datas.rows[idxArr[i].idx];
		}

		delete[] datas.rows;
		datas.rows = newRows;
		delete[] idxArr;
	}
	else
	{
		StrIdx* idxArr = new StrIdx[datas.rowSize];
		for (int i = 0; i < datas.rowSize; ++i)
		{
			idxArr[i].val = datas.rows[i][colIdx];
			idxArr[i].idx = i;
		}
		sort(idxArr, idxArr + datas.rowSize, [](const StrIdx& a, const StrIdx& b)
			{
				return strcmp(a.val, b.val) < 0;
			});

		char*** newRows = new char** [datas.rowSize];
		for (int i = 0; i < datas.rowSize; ++i)
		{
			newRows[i] = datas.rows[idxArr[i].idx];
		}

		delete[] datas.rows;
		datas.rows = newRows;
		delete[] idxArr;
	}
}

int getLowerBound(StrIdx* arr, int size, const char* target)
{
	int left = 0;
	int right = size - 1;
	int res = size;

	while (left <= right)
	{
		int mid = left + (right - left) / 2;
		if (strcmp(arr[mid].val, target) >= 0)
		{
			res = mid;
			right = mid - 1;
		}
		else
		{
			left = mid + 1;
		}
	}

	return res;
}

int getUpperBound(StrIdx* arr, int size, const char* target)
{
	int left = 0;
	int right = size - 1;
	int res = size;

	while (left <= right)
	{
		int mid = left + (right - left) / 2;
		if (strcmp(arr[mid].val, target) > 0)
		{
			res = mid;
			right = mid - 1;
		}
		else
		{
			left = mid + 1;
		}
	}

	return res;
}

void getUserJourney(const DataArray& datas, StrIdx* userIdxArr, const string& user_id, unsigned long long startTimestamp, unsigned long long endTimestamp)
{
	int timeIdx = datas.getColumnIndex("timestamp");
	int devIdx = datas.getColumnIndex("device_id");
	int appIdx = datas.getColumnIndex("app_id");
	int resIdx = datas.getColumnIndex("resource_id");
	if (!userIdxArr || timeIdx == -1 || devIdx == -1 || appIdx == -1 || resIdx == -1)
	{
		cout << "Khong du thong tin de hien thi!\n";
		return;
	}

	cout << "\nHanh trinh truy xuat cua User " << user_id << " tu " << startTimestamp << " den " << endTimestamp << " la:\n";
	int startIdx = getLowerBound(userIdxArr, datas.rowSize, user_id.c_str());
	int endIdx = getUpperBound(userIdxArr, datas.rowSize, user_id.c_str());

	bool any = false;
	for (int i = startIdx; i < endIdx; ++i)
	{
		char** rowData = datas.rows[userIdxArr[i].idx];
		unsigned long long timestamp = fast_stoull(rowData[timeIdx]);

		if (timestamp >= startTimestamp && timestamp <= endTimestamp)
		{
			cout << "[" << timestamp << "] " << rowData[devIdx] << " -> " << rowData[appIdx] << " -> " << rowData[resIdx] << endl;
			any = true;
		}
	}

	if (!any)
	{
		cout << "User " << user_id << " khong co truy xuat nao trong khoang thoi gian nay.\n";
	}
}

void getResourceJourney(const DataArray& datas, StrIdx* resourceIdxArr, const string& resource_id, unsigned long long startTimestamp, unsigned long long endTimestamp)
{
	int timeIdx = datas.getColumnIndex("timestamp");
	int userIdx = datas.getColumnIndex("user_id");
	int devIdx = datas.getColumnIndex("device_id");
	int appIdx = datas.getColumnIndex("app_id");

	if (!resourceIdxArr || timeIdx == -1 || userIdx == -1 || devIdx == -1 || appIdx == -1)
	{
		cout << "Khong du thong tin de hien thi!\n";
		return;
	}

	cout << "\nHanh trinh truy xuat cua Resource " << resource_id << " tu " << startTimestamp << " den " << endTimestamp << " la:\n";
	int startIdx = getLowerBound(resourceIdxArr, datas.rowSize, resource_id.c_str());
	int endIdx = getUpperBound(resourceIdxArr, datas.rowSize, resource_id.c_str());

	bool any = false;
	for (int i = startIdx; i < endIdx; ++i)
	{
		char** rowData = datas.rows[resourceIdxArr[i].idx];
		unsigned long long timestamp = fast_stoull(rowData[timeIdx]);

		if (timestamp >= startTimestamp && timestamp <= endTimestamp)
		{
			cout << "[" << timestamp << "] " << rowData[userIdx] << " -> " << rowData[devIdx] << " -> " << rowData[appIdx] << endl;
			any = true;
		}
	}

	if (!any)
	{
		cout << "Resource " << resource_id << " khong co truy xuat nao trong khoang thoi gian nay.\n";
	}
}

void getTop10Resources(const DataArray& datas, StrIdx* resourceIdxArr, unsigned long long startTimestamp, unsigned long long endTimestamp)
{
	int timeIdx = datas.getColumnIndex("timestamp");

	if (!resourceIdxArr || timeIdx == -1)
	{
		cout << "Khong du thong tin de hien thi!\n";
		return;
	}
	
	cout << "\nTop 10 Resources duoc truy xuat nhieu nhat tu " << startTimestamp << " den " << endTimestamp << " la:\n";
	StrIdx* resourceArr = new StrIdx[datas.rowSize];
	int size = 0;
	int idx = 0;
	while (idx < datas.rowSize)
	{
		const char* resource = resourceIdxArr[idx].val;
		if (!resource || resource[0] == '\0')
		{
			++idx;
			continue;
		}

		int cnt = 0;
		while (idx < datas.rowSize && strcmp(resourceIdxArr[idx].val, resource) == 0)
		{
			unsigned long long timestamp = fast_stoull(datas.rows[resourceIdxArr[idx].idx][timeIdx]);
			if (timestamp >= startTimestamp && timestamp <= endTimestamp)
			{
				++cnt;
			}
			++idx;
		}
		resourceArr[size].val = resource;
		resourceArr[size].idx = cnt;
		++size;
	}

	if (size == 0)
	{
		cout << "Khong co tai nguyen nao duoc truy xuat trong khoang thoi gian nay.\n";
		delete[] resourceArr;
		return;
	}

	stable_sort(resourceArr, resourceArr + size, [](const StrIdx& a, const StrIdx& b)
		{
			return a.idx > b.idx;
		});

	for (int i = 0; i < ((size < 10) ? size : 10); ++i)
	{
		cout << i + 1 << ". " << resourceArr[i].val << ": " << resourceArr[i].idx << " luot truy cap\n";
	}

	delete[] resourceArr;
}
