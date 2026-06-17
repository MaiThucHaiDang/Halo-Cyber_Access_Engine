#pragma once
#include "DataArray.h"

struct UserIdx
{
    const char* user_id;
    unsigned long long timestamp;
    int idx;
};

struct DeviceIdx
{
    const char* device_id;
    unsigned long long timestamp;
    int idx;
};

void detectAnomalies(const DataArray& datas);
