#include "AnomalyDetector.h"
#include <iostream>
#include <algorithm>
#include <cstring>
#include <string>
#include <thread>
#include <vector>
#include "Engine.h"
using namespace std;

struct UniqueStringList
{
    const char** items;
    int count;
    int capacity;

    UniqueStringList(int cap)
    {
        capacity = cap;
        items = new const char*[capacity];
        count = 0;
    }

    ~UniqueStringList()
    {
        delete[] items;
    }

    void add(const char* str)
    {
        for (int i = 0; i < count; ++i)
        {
            if (strcmp(items[i], str) == 0)
            {
                return;
            }
        }
        if (count < capacity)
        {
            items[count++] = str;
        }
    }

    int getCount() const
    {
        return count;
    }
};

void detectAnomalies(const DataArray& datas)
{
    int userIdxCol = datas.getColumnIndex("user_id");
    int devIdxCol = datas.getColumnIndex("device_id");
    int appIdxCol = datas.getColumnIndex("app_id");
    int resIdxCol = datas.getColumnIndex("resource_id");
    int evtIdxCol = datas.getColumnIndex("event_type");
    int locIdxCol = datas.getColumnIndex("location");
    int timeIdxCol = datas.getColumnIndex("timestamp");

    UserIdx* uArr = new UserIdx[datas.rowSize];
    DeviceIdx* dArr = new DeviceIdx[datas.rowSize];

    thread prepareThread([&]()
    {
        for (int i = 0; i < datas.rowSize; ++i)
        {
            uArr[i].user_id = datas.rows[i][userIdxCol];
            uArr[i].timestamp = fast_stoull(datas.rows[i][timeIdxCol]);
            uArr[i].idx = i;

            dArr[i].device_id = datas.rows[i][devIdxCol];
            dArr[i].timestamp = uArr[i].timestamp;
            dArr[i].idx = i;
        }

        thread t1([&]()
        {
            stable_sort(uArr, uArr + datas.rowSize, [](const UserIdx& a, const UserIdx& b)
            {
                return strcmp(a.user_id, b.user_id) < 0;
            });
        });

        thread t2([&]()
        {
            stable_sort(dArr, dArr + datas.rowSize, [](const DeviceIdx& a, const DeviceIdx& b)
            {
                return strcmp(a.device_id, b.device_id) < 0;
            });
        });

        t1.join();
        t2.join();
    });

    string choice = "";
    while (true)
    {
        cout << "\n======================================================================\n";
        cout << "                      MENU PHAT HIEN BAT THUONG\n";
        cout << "======================================================================\n";
        cout << "1. Mot nguoi dung dang nhap tu qua nhieu thiet bi trong thoi gian ngan\n";
        cout << "2. Nguoi dung login that bai lien tuc\n";
        cout << "3. Mot thiet bi dot ngot truy cap qua nhieu resource khac nhau\n";
        cout << "4. Truy cap ngoai gio lam viec\n";
        cout << "5. Nguoi dung xuat hien o nhieu quoc gia khong hop ly\n";
        cout << "6. Nguoi dung lien tuc doi vi tri dia ly\n";
        cout << "7. Phien lam viec dai bat thuong\n";
        cout << "8. Nguoi dung tao nhieu phien lam viec lien tuc bat thuong\n";
        cout << "9. Phien lam viec co chuoi hanh dong nguy hiem dang ngo\n";
        cout << "10. Co dang nhap\n";
        cout << "11. Nguoi dung im lang rat lau roi dot ngot hoat dong manh\n";
        cout << "0. Quay lai menu chinh\n";
        cout << "======================================================================\n";
        cout << "Moi thay nhap lua chon: ";
        cin >> choice;

        if (choice == "0" || choice == "1" || choice == "2" || choice == "3" ||
            choice == "4" || choice == "5" || choice == "6" || choice == "7" ||
            choice == "8" || choice == "9" || choice == "10" || choice == "11")
        {
            if (prepareThread.joinable())
            {
                prepareThread.join();
            }
        }
        else
        {
            cout << "Lua chon khong hop le.\n";
            system("pause");
            system("cls");
            continue;
        }

        if (choice == "0")
        {
            break;
        }

        vector<string> results;

        if (choice == "1")
        {
            int start = 0;
            while (start < datas.rowSize)
            {
                int end = start + 1;
                while (end < datas.rowSize && strcmp(uArr[end].user_id, uArr[start].user_id) == 0)
                {
                    end++;
                }
                
                for (int i = start; i < end; ++i)
                {
                    if (strcmp(datas.rows[uArr[i].idx][evtIdxCol], "LOGIN") == 0)
                    {
                        UniqueStringList udevs(10);
                        udevs.add(datas.rows[uArr[i].idx][devIdxCol]);
                        int m = i + 1;
                        for (; m < end; ++m)
                        {
                            if (uArr[m].timestamp - uArr[i].timestamp > 3600)
                            {
                                break;
                            }
                            if (strcmp(datas.rows[uArr[m].idx][evtIdxCol], "LOGIN") == 0)
                            {
                                udevs.add(datas.rows[uArr[m].idx][devIdxCol]);
                            }
                        }
                        if (udevs.getCount() >= 3)
                        {
                            results.push_back("User " + string(uArr[start].user_id) + " dung " + to_string(udevs.getCount()) + " thiet bi luc " + to_string(uArr[i].timestamp));
                            i = m - 1;
                        }
                    }
                }
                start = end;
            }
        }
        else if (choice == "2")
        {
            for (int i = 0; i < datas.rowSize;)
            {
                int j = i;
                while (j < datas.rowSize && strcmp(uArr[j].user_id, uArr[i].user_id) == 0)
                {
                    j++;
                }
                int failedCount = 0;
                unsigned long long firstFailTime = 0;
                unsigned long long lastFailTime = 0;
                for (int k = i; k < j; ++k)
                {
                    if (strcmp(datas.rows[uArr[k].idx][evtIdxCol], "FAILED_LOGIN") == 0)
                    {
                        if (failedCount == 0)
                        {
                            firstFailTime = uArr[k].timestamp;
                        }
                        lastFailTime = uArr[k].timestamp;
                        failedCount++;
                    }
                    else if (strcmp(datas.rows[uArr[k].idx][evtIdxCol], "LOGIN") == 0)
                    {
                        if (failedCount >= 5)
                        {
                            results.push_back("User " + string(uArr[i].user_id) + " fail login lien tuc " + to_string(failedCount) + " lan (tu " + to_string(firstFailTime) + " den " + to_string(lastFailTime) + ")");
                        }
                        failedCount = 0;
                    }
                }
                if (failedCount >= 5)
                {
                    results.push_back("User " + string(uArr[i].user_id) + " fail login lien tuc " + to_string(failedCount) + " lan (tu " + to_string(firstFailTime) + " den " + to_string(lastFailTime) + ")");
                }
                i = j;
            }
        }
        else if (choice == "3")
        {
            for (int i = 0; i < datas.rowSize;)
            {
                int j = i;
                while (j < datas.rowSize && strcmp(dArr[j].device_id, dArr[i].device_id) == 0)
                {
                    j++;
                }
                for (int k = i; k < j; ++k)
                {
                    UniqueStringList ures(30);
                    ures.add(datas.rows[dArr[k].idx][resIdxCol]);
                    int m = k + 1;
                    for (; m < j; ++m)
                    {
                        if (dArr[m].timestamp - dArr[k].timestamp > 60)
                        {
                            break;
                        }
                        ures.add(datas.rows[dArr[m].idx][resIdxCol]);
                    }
                    if (ures.getCount() >= 10)
                    {
                        results.push_back("Device " + string(dArr[i].device_id) + " truy cap " + to_string(ures.getCount()) + " resource luc " + to_string(dArr[k].timestamp));
                        k = m - 1;
                    }
                }
                i = j;
            }
        }
        else if (choice == "4")
        {
            int startHour = 8, endHour = 17;
            for (int i = 0; i < datas.rowSize; ++i)
            {
                unsigned long long ts = fast_stoull(datas.rows[i][timeIdxCol]);
                long long secondsOfDay = ts % 86400;
                bool isOutside = false;
                if (startHour <= endHour)
                {
                    if (secondsOfDay < startHour * 3600 || secondsOfDay > endHour * 3600)
                    {
                        isOutside = true;
                    }
                }
                else
                {
                    if (secondsOfDay > endHour * 3600 && secondsOfDay < startHour * 3600)
                    {
                        isOutside = true;
                    }
                }

                if (isOutside)
                {
                    results.push_back("User " + string(datas.rows[i][userIdxCol]) + " hoat dong ngoai gio luc " + to_string(ts) + " (" + to_string(secondsOfDay / 3600) + "h)");
                }
            }
        }
        else if (choice == "5")
        {
            for (int i = 0; i < datas.rowSize;)
            {
                int j = i;
                while (j < datas.rowSize && strcmp(uArr[j].user_id, uArr[i].user_id) == 0)
                {
                    j++;
                }
                for (int k = i; k < j; ++k)
                {
                    UniqueStringList ulocs(10);
                    ulocs.add(datas.rows[uArr[k].idx][locIdxCol]);
                    int m = k + 1;
                    for (; m < j; ++m)
                    {
                        if (uArr[m].timestamp - uArr[k].timestamp > 86400)
                        {
                            break;
                        }
                        ulocs.add(datas.rows[uArr[m].idx][locIdxCol]);
                    }
                    if (ulocs.getCount() >= 4)
                    {
                        results.push_back("User " + string(uArr[i].user_id) + " luan chuyen " + to_string(ulocs.getCount()) + " quoc gia luc " + to_string(uArr[k].timestamp));
                        k = m - 1;
                    }
                }
                i = j;
            }
        }
        else if (choice == "6")
        {
            for (int i = 0; i < datas.rowSize;)
            {
                int j = i;
                while (j < datas.rowSize && strcmp(uArr[j].user_id, uArr[i].user_id) == 0)
                {
                    j++;
                }
                for (int k = i; k < j; ++k)
                {
                    int changes = 0;
                    const char* lastLoc = datas.rows[uArr[k].idx][locIdxCol];
                    int m = k + 1;
                    for (; m < j; ++m)
                    {
                        if (uArr[m].timestamp - uArr[k].timestamp > 86400)
                        {
                            break;
                        }
                        const char* curLoc = datas.rows[uArr[m].idx][locIdxCol];
                        if (strcmp(curLoc, lastLoc) != 0)
                        {
                            changes++;
                            lastLoc = curLoc;
                        }
                    }
                    if (changes >= 4)
                    {
                        results.push_back("User " + string(uArr[i].user_id) + " doi vi tri " + to_string(changes) + " lan trong ngay " + to_string(uArr[k].timestamp));
                        k = m - 1;
                    }
                }
                i = j;
            }
        }
        else if (choice == "7")
        {
            for (int i = 0; i < datas.rowSize;)
            {
                int j = i;
                while (j < datas.rowSize && strcmp(uArr[j].user_id, uArr[i].user_id) == 0)
                {
                    j++;
                }
                unsigned long long loginTs = 0;
                for (int k = i; k < j; ++k)
                {
                    const char* evt = datas.rows[uArr[k].idx][evtIdxCol];
                    if (strcmp(evt, "LOGIN") == 0)
                    {
                        loginTs = uArr[k].timestamp;
                    }
                    else if (strcmp(evt, "LOGOUT") == 0 && loginTs != 0)
                    {
                        if (uArr[k].timestamp - loginTs > 28800)
                        {
                            results.push_back("User " + string(uArr[i].user_id) + " co phien lam viec " + to_string((uArr[k].timestamp - loginTs) / 3600) + " gio");
                        }
                        loginTs = 0;
                    }
                }
                i = j;
            }
        }
        else if (choice == "8")
        {
            for (int i = 0; i < datas.rowSize;)
            {
                int j = i;
                while (j < datas.rowSize && strcmp(uArr[j].user_id, uArr[i].user_id) == 0)
                {
                    j++;
                }
                for (int k = i; k < j; ++k)
                {
                    if (strcmp(datas.rows[uArr[k].idx][evtIdxCol], "LOGIN") == 0)
                    {
                        int logins = 1;
                        int m = k + 1;
                        for (; m < j; ++m)
                        {
                            if (uArr[m].timestamp - uArr[k].timestamp > 86400)
                            {
                                break;
                            }
                            if (strcmp(datas.rows[uArr[m].idx][evtIdxCol], "LOGIN") == 0)
                            {
                                logins++;
                            }
                        }
                        if (logins >= 5)
                        {
                            results.push_back("User " + string(uArr[i].user_id) + " dang nhap " + to_string(logins) + " lan / ngay tu luc " + to_string(uArr[k].timestamp));
                            k = m - 1;
                        }
                    }
                }
                i = j;
            }
        }
        else if (choice == "9")
        {
            for (int i = 0; i < datas.rowSize;)
            {
                int j = i;
                while (j < datas.rowSize && strcmp(uArr[j].user_id, uArr[i].user_id) == 0)
                {
                    j++;
                }
                bool inSession = false;
                bool hasAdmin = false;
                int downloads = 0;
                unsigned long long adminTs = 0;
                for (int k = i; k < j; ++k)
                {
                    const char* evt = datas.rows[uArr[k].idx][evtIdxCol];
                    if (strcmp(evt, "LOGIN") == 0)
                    {
                        inSession = true;
                        hasAdmin = false;
                        downloads = 0;
                    }
                    else if (strcmp(evt, "LOGOUT") == 0)
                    {
                        inSession = false;
                    }
                    else if (inSession && strcmp(evt, "ADMIN_ACTION") == 0)
                    {
                        hasAdmin = true;
                        adminTs = uArr[k].timestamp;
                    }
                    else if (inSession && hasAdmin && strcmp(evt, "DOWNLOAD") == 0)
                    {
                        downloads++;
                        if (downloads >= 3)
                        {
                            results.push_back("User " + string(uArr[i].user_id) + " thuc hien ADMIN_ACTION (" + to_string(adminTs) + ") roi DOWNLOAD " + to_string(downloads) + " lan");
                            hasAdmin = false;
                        }
                    }
                }
                i = j;
            }
        }
        else if (choice == "10")
        {
            for (int i = 0; i < datas.rowSize;)
            {
                int j = i;
                while (j < datas.rowSize && strcmp(uArr[j].user_id, uArr[i].user_id) == 0)
                {
                    j++;
                }
                int fails = 0;
                for (int k = i; k < j; ++k)
                {
                    const char* evt = datas.rows[uArr[k].idx][evtIdxCol];
                    if (strcmp(evt, "FAILED_LOGIN") == 0)
                    {
                        fails++;
                    }
                    else if (strcmp(evt, "LOGIN") == 0)
                    {
                        if (fails >= 4)
                        {
                            results.push_back("User " + string(uArr[i].user_id) + " fail " + to_string(fails) + " lan truoc khi dang nhap thanh cong luc " + to_string(uArr[k].timestamp));
                        }
                        fails = 0;
                    }
                }
                i = j;
            }
        }
        else if (choice == "11")
        {
            for (int i = 0; i < datas.rowSize;)
            {
                int j = i;
                while (j < datas.rowSize && strcmp(uArr[j].user_id, uArr[i].user_id) == 0)
                {
                    j++;
                }
                for (int k = i + 1; k < j; ++k)
                {
                    if (uArr[k].timestamp - uArr[k - 1].timestamp > 604800)
                    {
                        int actions = 0;
                        int m = k;
                        for (; m < j; ++m)
                        {
                            if (uArr[m].timestamp - uArr[k].timestamp > 3600)
                            {
                                break;
                            }
                            actions++;
                        }
                        if (actions >= 10)
                        {
                            results.push_back("User " + string(uArr[i].user_id) + " thuc giac voi " + to_string(actions) + " hanh dong luc " + to_string(uArr[k].timestamp));
                            k = m - 1;
                        }
                    }
                }
                i = j;
            }
        }

        int total = results.size();
        if (total == 0)
        {
            cout << "Khong phat hien truong hop nao.\n";
        }
        else
        {
            int printed = 0;
            int chunk = 1000;
            while (printed < total)
            {
                int endPrint = min(printed + chunk, total);
                for (int i = printed; i < endPrint; ++i)
                {
                    cout << results[i] << "\n";
                }
                printed = endPrint;

                if (printed < total)
                {
                    bool validOpt = false;
                    while (!validOpt)
                    {
                        cout << "\n==================================================\n";
                        cout << "Da hien thi " << printed << "/" << total << " truong hop.\n";
                        cout << "1. Xem tiep 1000 dong\n";
                        cout << "2. Xem tiep n dong (nhap n)\n";
                        cout << "0. Khong xem nua\n";
                        cout << "Chon: ";
                        string opt;
                        cin >> opt;

                        if (opt == "1")
                        {
                            chunk = 1000;
                            validOpt = true;
                        }
                        else if (opt == "2")
                        {
                            string nStr;
                            cout << "Nhap n: ";
                            cin >> nStr;
                            try
                            {
                                chunk = stoi(nStr);
                                if (chunk > 0)
                                {
                                    validOpt = true;
                                }
                                else
                                {
                                    cout << "Nhap sai moi nhap lai.\n";
                                }
                            }
                            catch (...)
                            {
                                cout << "Nhap sai moi nhap lai.\n";
                            }
                        }
                        else if (opt == "0")
                        {
                            chunk = 0;
                            validOpt = true;
                        }
                        else
                        {
                            cout << "Nhap sai moi nhap lai.\n";
                        }
                    }

                    if (chunk == 0)
                    {
                        break;
                    }
                }
            }
            cout << "\n=> Tong cong phat hien: " << total << " truong hop.\n";
        }
    }

    delete[] uArr;
    delete[] dArr;
}
