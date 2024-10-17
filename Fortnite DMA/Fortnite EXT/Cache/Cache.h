#pragma once
#include <memory>
#include <string>

class Cache {
private:
    static std::unique_ptr<Cache> instance;

    Cache() {}

public:

    // ProcessInfo
    struct ProcessInfo {
        __int32 TargetPID;
        __int64 BaseAddress;
        size_t base_size;
        std::string ProcessName = "FortniteClient-Win64-Shipping.exe";
    };

    ProcessInfo ProcessInfo;

    Cache(const Cache&) = delete;
    Cache& operator=(const Cache&) = delete;

    static Cache& getInstance() {
        if (!instance) {
            instance.reset(new Cache());
        }
        return *instance;
    }
};