#include "AnomalyDetector.h"
#include <iostream>
#include <algorithm>
#include <cstring>

using namespace std;

extern unsigned long long fast_stoull(const char* s);

struct UserEventIdx {
    const char* user_id;
    unsigned long long timestamp;
    int originalIdx;
};

struct DeviceEventIdx {
    const char* device_id;
    unsigned long long timestamp;
    int originalIdx;
};

struct UniqueStringList {
    const char** arr;
    int size;
    int capacity;

    UniqueStringList(int cap) {
        capacity = cap;
        size = 0;
        arr = new const char*[capacity];
    }

    ~UniqueStringList() {
        delete[] arr;
    }

    void add(const char* s) {
        for (int i = 0; i < size; ++i) {
            if (strcmp(arr[i], s) == 0) return;
        }
        if (size < capacity) {
            arr[size++] = s;
        }
    }

    void clear() {
        size = 0;
    }
    
    int getCount() const {
        return size;
    }
};

void detectAnomalies(const DataArray& datas) {
    int userIdxCol = datas.getColumnIndex("user_id");
    int devIdxCol = datas.getColumnIndex("device_id");
    int appIdxCol = datas.getColumnIndex("app_id");
    int resIdxCol = datas.getColumnIndex("resource_id");
    int evtIdxCol = datas.getColumnIndex("event_type");
    int locIdxCol = datas.getColumnIndex("location");
    int timeIdxCol = datas.getColumnIndex("timestamp");

    if (userIdxCol == -1 || devIdxCol == -1 || resIdxCol == -1 || evtIdxCol == -1 || locIdxCol == -1 || timeIdxCol == -1) {
        cout << "Khong du cot de phan tich bat thuong!\n";
        return;
    }

    cout << "\nDang phan tich cac hanh vi bat thuong (co the mat vai giay)...\n";

    UserEventIdx* uArr = new UserEventIdx[datas.rowSize];
    DeviceEventIdx* dArr = new DeviceEventIdx[datas.rowSize];

    for (int i = 0; i < datas.rowSize; ++i) {
        uArr[i].user_id = datas.rows[i][userIdxCol];
        uArr[i].timestamp = fast_stoull(datas.rows[i][timeIdxCol]);
        uArr[i].originalIdx = i;

        dArr[i].device_id = datas.rows[i][devIdxCol];
        dArr[i].timestamp = uArr[i].timestamp;
        dArr[i].originalIdx = i;
    }

    stable_sort(uArr, uArr + datas.rowSize, [](const UserEventIdx& a, const UserEventIdx& b) {
        int cmp = strcmp(a.user_id, b.user_id);
        if (cmp != 0) return cmp < 0;
        return a.timestamp < b.timestamp;
    });

    stable_sort(dArr, dArr + datas.rowSize, [](const DeviceEventIdx& a, const DeviceEventIdx& b) {
        int cmp = strcmp(a.device_id, b.device_id);
        if (cmp != 0) return cmp < 0;
        return a.timestamp < b.timestamp;
    });

    int maxPrint = 5;

    // 1. Quá nhiều device trong thời gian ngắn (>=3 devices trong 1h)
    cout << "\n--- 1. Nguoi dung dang nhap nhieu thiet bi trong thoi gian ngan ---\n";
    int p = 0;
    for (int i = 0; i < datas.rowSize;) {
        int j = i;
        while (j < datas.rowSize && strcmp(uArr[j].user_id, uArr[i].user_id) == 0) j++;
        for (int k = i; k < j && p < maxPrint; ++k) {
            if (strcmp(datas.rows[uArr[k].originalIdx][evtIdxCol], "LOGIN") == 0) {
                UniqueStringList udevs(10);
                udevs.add(datas.rows[uArr[k].originalIdx][devIdxCol]);
                for (int m = k + 1; m < j; ++m) {
                    if (uArr[m].timestamp - uArr[k].timestamp > 3600) break;
                    if (strcmp(datas.rows[uArr[m].originalIdx][evtIdxCol], "LOGIN") == 0) {
                        udevs.add(datas.rows[uArr[m].originalIdx][devIdxCol]);
                    }
                }
                if (udevs.getCount() >= 3) {
                    cout << "User " << uArr[i].user_id << " dung " << udevs.getCount() << " thiet bi luc " << uArr[k].timestamp << "\n";
                    p++;
                    k += 10; // skip a bit
                }
            }
        }
        i = j;
    }
    if (p == 0) cout << "Khong phat hien.\n";

    // 2. Login thất bại liên tục (>= 5 lần)
    cout << "\n--- 2. Nguoi dung login that bai lien tuc ---\n";
    p = 0;
    for (int i = 0; i < datas.rowSize;) {
        int j = i;
        while (j < datas.rowSize && strcmp(uArr[j].user_id, uArr[i].user_id) == 0) j++;
        int failedCount = 0;
        for (int k = i; k < j && p < maxPrint; ++k) {
            if (strcmp(datas.rows[uArr[k].originalIdx][evtIdxCol], "FAILED_LOGIN") == 0) {
                failedCount++;
                if (failedCount >= 5) {
                    cout << "User " << uArr[i].user_id << " fail login " << failedCount << " lan luc " << uArr[k].timestamp << "\n";
                    p++;
                    failedCount = 0; // reset
                }
            } else if (strcmp(datas.rows[uArr[k].originalIdx][evtIdxCol], "LOGIN") == 0) {
                failedCount = 0;
            }
        }
        i = j;
    }
    if (p == 0) cout << "Khong phat hien.\n";

    // 3. Thiết bị truy cập nhiều resource (>= 10 resource trong 60s)
    cout << "\n--- 3. Thiet bi truy cap qua nhieu resource dot ngot ---\n";
    p = 0;
    for (int i = 0; i < datas.rowSize;) {
        int j = i;
        while (j < datas.rowSize && strcmp(dArr[j].device_id, dArr[i].device_id) == 0) j++;
        for (int k = i; k < j && p < maxPrint; ++k) {
            UniqueStringList ures(30);
            ures.add(datas.rows[dArr[k].originalIdx][resIdxCol]);
            for (int m = k + 1; m < j; ++m) {
                if (dArr[m].timestamp - dArr[k].timestamp > 60) break;
                ures.add(datas.rows[dArr[m].originalIdx][resIdxCol]);
            }
            if (ures.getCount() >= 10) {
                cout << "Device " << dArr[i].device_id << " truy cap " << ures.getCount() << " resource luc " << dArr[k].timestamp << "\n";
                p++;
                k += ures.getCount(); // skip
            }
        }
        i = j;
    }
    if (p == 0) cout << "Khong phat hien.\n";

    // 4. Truy cập ngoài giờ làm việc (truớc 6h sáng hoặc sau 20h tối)
    cout << "\n--- 4. Truy cap ngoai gio lam viec (20h - 6h) ---\n";
    p = 0;
    for (int i = 0; i < datas.rowSize && p < maxPrint; ++i) {
        unsigned long long ts = fast_stoull(datas.rows[i][timeIdxCol]);
        long long secondsOfDay = ts % 86400;
        if (secondsOfDay < 6 * 3600 || secondsOfDay > 20 * 3600) {
            cout << "User " << datas.rows[i][userIdxCol] << " hoat dong ngoai gio luc " << ts << " (" << secondsOfDay/3600 << "h)\n";
            p++;
        }
    }
    if (p == 0) cout << "Khong phat hien.\n";

    // 5. User xuất hiện ở nhiều quốc gia không hợp lý (< 4h di chuyển)
    cout << "\n--- 5. User xuat hien o cac quoc gia bat hop ly ---\n";
    p = 0;
    for (int i = 0; i < datas.rowSize;) {
        int j = i;
        while (j < datas.rowSize && strcmp(uArr[j].user_id, uArr[i].user_id) == 0) j++;
        for (int k = i; k < j - 1 && p < maxPrint; ++k) {
            const char* loc1 = datas.rows[uArr[k].originalIdx][locIdxCol];
            const char* loc2 = datas.rows[uArr[k+1].originalIdx][locIdxCol];
            if (strcmp(loc1, loc2) != 0 && uArr[k+1].timestamp - uArr[k].timestamp < 4 * 3600) {
                cout << "User " << uArr[i].user_id << " tu " << loc1 << " sang " << loc2 << " trong " << (uArr[k+1].timestamp - uArr[k].timestamp)/60 << " phut\n";
                p++;
            }
        }
        i = j;
    }
    if (p == 0) cout << "Khong phat hien.\n";

    // 6. User liên tục đổi vị trí (>= 4 lần / 24h)
    cout << "\n--- 6. User lien tuc doi vi tri dia ly ---\n";
    p = 0;
    for (int i = 0; i < datas.rowSize;) {
        int j = i;
        while (j < datas.rowSize && strcmp(uArr[j].user_id, uArr[i].user_id) == 0) j++;
        for (int k = i; k < j && p < maxPrint; ++k) {
            int changes = 0;
            const char* lastLoc = datas.rows[uArr[k].originalIdx][locIdxCol];
            for (int m = k + 1; m < j; ++m) {
                if (uArr[m].timestamp - uArr[k].timestamp > 86400) break;
                const char* curLoc = datas.rows[uArr[m].originalIdx][locIdxCol];
                if (strcmp(curLoc, lastLoc) != 0) {
                    changes++;
                    lastLoc = curLoc;
                }
            }
            if (changes >= 4) {
                cout << "User " << uArr[i].user_id << " doi vi tri " << changes << " lan trong ngay " << uArr[k].timestamp << "\n";
                p++;
                k += changes;
            }
        }
        i = j;
    }
    if (p == 0) cout << "Khong phat hien.\n";

    // 7. Phiên làm việc dài bất thường (> 24h)
    cout << "\n--- 7. Phien lam viec dai bat thuong (> 24h) ---\n";
    p = 0;
    for (int i = 0; i < datas.rowSize;) {
        int j = i;
        while (j < datas.rowSize && strcmp(uArr[j].user_id, uArr[i].user_id) == 0) j++;
        unsigned long long loginTs = 0;
        for (int k = i; k < j && p < maxPrint; ++k) {
            const char* evt = datas.rows[uArr[k].originalIdx][evtIdxCol];
            if (strcmp(evt, "LOGIN") == 0) {
                loginTs = uArr[k].timestamp;
            } else if (strcmp(evt, "LOGOUT") == 0 && loginTs != 0) {
                if (uArr[k].timestamp - loginTs > 86400) {
                    cout << "User " << uArr[i].user_id << " co phien lam viec " << (uArr[k].timestamp - loginTs)/3600 << " gio\n";
                    p++;
                }
                loginTs = 0;
            }
        }
        i = j;
    }
    if (p == 0) cout << "Khong phat hien.\n";

    // 8. Tạo nhiều phiên làm việc liên tục (>= 10 logins trong 24h)
    cout << "\n--- 8. Tao nhieu phien lam viec lien tuc (>= 10 phien / ngay) ---\n";
    p = 0;
    for (int i = 0; i < datas.rowSize;) {
        int j = i;
        while (j < datas.rowSize && strcmp(uArr[j].user_id, uArr[i].user_id) == 0) j++;
        for (int k = i; k < j && p < maxPrint; ++k) {
            if (strcmp(datas.rows[uArr[k].originalIdx][evtIdxCol], "LOGIN") == 0) {
                int logins = 1;
                for (int m = k + 1; m < j; ++m) {
                    if (uArr[m].timestamp - uArr[k].timestamp > 86400) break;
                    if (strcmp(datas.rows[uArr[m].originalIdx][evtIdxCol], "LOGIN") == 0) logins++;
                }
                if (logins >= 10) {
                    cout << "User " << uArr[i].user_id << " dang nhap " << logins << " lan / ngay tu luc " << uArr[k].timestamp << "\n";
                    p++;
                    k += logins;
                }
            }
        }
        i = j;
    }
    if (p == 0) cout << "Khong phat hien.\n";

    // 9. Hành động nguy hiểm: ADMIN_ACTION theo sau bởi >=3 DOWNLOAD trong 1 phiên
    cout << "\n--- 9. Hanh dong nguy hiem (ADMIN -> nhieu DOWNLOAD) ---\n";
    p = 0;
    for (int i = 0; i < datas.rowSize;) {
        int j = i;
        while (j < datas.rowSize && strcmp(uArr[j].user_id, uArr[i].user_id) == 0) j++;
        bool inSession = false;
        bool hasAdmin = false;
        int downloads = 0;
        unsigned long long adminTs = 0;
        for (int k = i; k < j && p < maxPrint; ++k) {
            const char* evt = datas.rows[uArr[k].originalIdx][evtIdxCol];
            if (strcmp(evt, "LOGIN") == 0) {
                inSession = true; hasAdmin = false; downloads = 0;
            } else if (strcmp(evt, "LOGOUT") == 0) {
                inSession = false;
            } else if (inSession && strcmp(evt, "ADMIN_ACTION") == 0) {
                hasAdmin = true; adminTs = uArr[k].timestamp;
            } else if (inSession && hasAdmin && strcmp(evt, "DOWNLOAD") == 0) {
                downloads++;
                if (downloads >= 3) {
                    cout << "User " << uArr[i].user_id << " thuc hien ADMIN_ACTION (" << adminTs << ") roi DOWNLOAD " << downloads << " lan\n";
                    p++;
                    hasAdmin = false; // reset to avoid spamming
                }
            }
        }
        i = j;
    }
    if (p == 0) cout << "Khong phat hien.\n";

    // 10. Cố đăng nhập (thất bại nhiều lần rồi thành công)
    cout << "\n--- 10. (Nang cao) Co dang nhap (>=4 failed, roi success) ---\n";
    p = 0;
    for (int i = 0; i < datas.rowSize;) {
        int j = i;
        while (j < datas.rowSize && strcmp(uArr[j].user_id, uArr[i].user_id) == 0) j++;
        int fails = 0;
        for (int k = i; k < j && p < maxPrint; ++k) {
            const char* evt = datas.rows[uArr[k].originalIdx][evtIdxCol];
            if (strcmp(evt, "FAILED_LOGIN") == 0) {
                fails++;
            } else if (strcmp(evt, "LOGIN") == 0) {
                if (fails >= 4) {
                    cout << "User " << uArr[i].user_id << " fail " << fails << " lan truoc khi dang nhap thanh cong luc " << uArr[k].timestamp << "\n";
                    p++;
                }
                fails = 0;
            } else {
                // Ignore other events
            }
        }
        i = j;
    }
    if (p == 0) cout << "Khong phat hien.\n";

    // 11. Im lặng lâu rồi hoạt động mạnh (> 7 ngày im lặng, >= 10 action trong 1h)
    cout << "\n--- 11. (Nang cao) Im lang lau roi hoat dong manh ---\n";
    p = 0;
    for (int i = 0; i < datas.rowSize;) {
        int j = i;
        while (j < datas.rowSize && strcmp(uArr[j].user_id, uArr[i].user_id) == 0) j++;
        for (int k = i + 1; k < j && p < maxPrint; ++k) {
            if (uArr[k].timestamp - uArr[k-1].timestamp > 7 * 86400) {
                int acts = 1;
                for (int m = k + 1; m < j; ++m) {
                    if (uArr[m].timestamp - uArr[k].timestamp > 3600) break;
                    acts++;
                }
                if (acts >= 10) {
                    cout << "User " << uArr[i].user_id << " im lang " << (uArr[k].timestamp - uArr[k-1].timestamp)/86400 << " ngay, sau do co " << acts << " thao tac trong 1h luc " << uArr[k].timestamp << "\n";
                    p++;
                }
            }
        }
        i = j;
    }
    if (p == 0) cout << "Khong phat hien.\n";

    cout << "\nHoan tat phan tich bat thuong.\n";

    delete[] uArr;
    delete[] dArr;
}
