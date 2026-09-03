#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include "filenumbersync.hpp"

#define SEC_BETWEEN_FILESYNC 60

FileNumberSync::FileNumberSync(std::string path) {
    filepath = path;
    number = fileValue();
    last = std::chrono::steady_clock::now();
}

unsigned long long FileNumberSync::value() {
    return number;
}

void FileNumberSync::increment() {
    number++;
    writeCheck();
}

void FileNumberSync::decrement() {
    number--;
    writeCheck();
}

unsigned long long FileNumberSync::fileValue() {
    std::ifstream file(filepath);
    if(!file.is_open()) {
        std::cerr << "Could not open fileNumberSync file at " << filepath << "\n";
        return 0;
    }

    std::string raw;
    std::getline(file, raw);

    return std::stoull(raw);
}

void FileNumberSync::writeCheck() {
    auto now = std::chrono::steady_clock::now();
    if(last.time_since_epoch() + std::chrono::seconds(SEC_BETWEEN_FILESYNC) > now.time_since_epoch()) return;
    last = now;

    std::ofstream file(filepath);
    if(!file.is_open()) {
        std::cerr << "Could not open fileNumberSync file at " << filepath << "\n";
        return;
    }

    file << number;
    file.close();
}