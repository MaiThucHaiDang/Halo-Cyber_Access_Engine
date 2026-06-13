#include <iostream>
#include <string>
#include <thread>
#include "DataArray.h"
#include "CSVReader.h"
#include "Engine.h"
#include "AnomalyDetector.h"
using namespace std;

StrIdx* userIdxArr = nullptr;
StrIdx* resourceIdxArr = nullptr;

bool isValidTimestamp(const string& str)
{
	if (str.empty() || str.length() > 20)
	{
		return false;
	}

	for (char c : str)
	{
		if (c < '0' || c > '9')
		{
			return false;
		}
	}

	if (str.length() == 20 && str > "18446744073709551615")
	{
		return false;
	}

	return true;
}

void menu()
{
	cout << "\n==================================================\n";
	cout << "            HALO - CYBER ACCESS ENGINE\n";
	cout << "==================================================\n";
	cout << "1. Hanh trinh Device -> App -> Resource cua 1 User\n";
	cout << "2. Hanh trinh User -> Device -> App cua 1 Resource\n";
	cout << "3. Top 10 Resource duoc truy xuat nhieu nhat\n";
	cout << "4. Phat hien cac hanh vi bat thuong (Danh gia cuoi ki)\n";
	cout << "5. Thoat va giai phong bo nho\n";
	cout << "==================================================\n";
	cout << "Moi thay nhap lua chon: ";
}

int main()
{
	DataArray datas;
	string dataset = "data.csv";
	cout << "Dang load du lieu tu " << dataset << endl;
	int size = loadData(dataset, datas);

	if (size == 0)
	{
		cout << "Khong the load du lieu.\n";
		system("pause");
		return 0;
	}

	cout << "\nDa load thanh cong " << size << " dong.\n";
	cout << "Moi dong co " << datas.columnSize << " cot gom:\n";
	cout << datas.columnNames[0];
	for (int i = 1; i < datas.columnSize; ++i)
	{
		cout << ", " << datas.columnNames[i];
	}
	cout << "\nDang tien xu ly (sap xep va danh chi muc song song)\n";

	sortByColumn(datas, "timestamp", true);
	thread t1([&]() { userIdxArr = sortIdxArr(datas, "user_id"); });
	thread t2([&]() { resourceIdxArr = sortIdxArr(datas, "resource_id"); });
	t1.join();
	t2.join();
	cout << "\nTien xu ly da xong.\n";

	string choice = "";
	while (true)
	{
		menu();
		cin >> choice;
		if (choice == "5")
		{
			break;
		}

		string id, startTimestamp, endTimestamp;
		if (choice == "1")
		{
			cout << "Moi thay nhap User ID: ";
			cin >> id;
			cout << "Moi thay nhap thoi gian bat dau: ";
			cin >> startTimestamp;
			if (!isValidTimestamp(startTimestamp))
			{
				cout << "Thoi gian bat dau khong hop le!\n";
				continue;
			}
			cout << "Moi thay nhap thoi gian ket thuc: ";
			cin >> endTimestamp;
			if (!isValidTimestamp(endTimestamp))
			{
				cout << "Thoi gian ket thuc khong hop le!\n";
				continue;
			}
			getUserJourney(datas, userIdxArr, id, fast_stoull(startTimestamp.c_str()), fast_stoull(endTimestamp.c_str()));
		}
		else if (choice == "2")
		{
			cout << "Moi thay nhap Resource ID: ";
			cin >> id;
			cout << "Moi thay nhap thoi gian bat dau: ";
			cin >> startTimestamp;
			if (!isValidTimestamp(startTimestamp))
			{
				cout << "Thoi gian bat dau khong hop le!\n";
				continue;
			}
			cout << "Moi thay nhap thoi gian ket thuc: ";
			cin >> endTimestamp;
			if (!isValidTimestamp(endTimestamp))
			{
				cout << "Thoi gian ket thuc khong hop le!\n";
				continue;
			}
			getResourceJourney(datas, resourceIdxArr, id, fast_stoull(startTimestamp.c_str()), fast_stoull(endTimestamp.c_str()));
		}
		else if (choice == "3")
		{
			cout << "Moi thay nhap thoi gian bat dau: ";
			cin >> startTimestamp;
			if (!isValidTimestamp(startTimestamp))
			{
				cout << "Thoi gian bat dau khong hop le!\n";
				continue;
			}
			cout << "Moi thay nhap thoi gian ket thuc: ";
			cin >> endTimestamp;
			if (!isValidTimestamp(endTimestamp))
			{
				cout << "Thoi gian ket thuc khong hop le!\n";
				continue;
			}
			getTop10Resources(datas, resourceIdxArr, fast_stoull(startTimestamp.c_str()), fast_stoull(endTimestamp.c_str()));
		}
		else if (choice == "4")
		{
			detectAnomalies(datas);
		}
		else
		{
			cout << "Lua chon cua thay khong hop le.\n";
		}
	}

	delete[] userIdxArr;
	delete[] resourceIdxArr;
	cout << "Da thu hoi cac du lieu duoc cap phat dong.\n";
	system("pause");

	return 0;
}
