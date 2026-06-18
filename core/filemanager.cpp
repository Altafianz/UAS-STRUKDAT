#include "filemanager.h"

#include <fstream>
#include <sstream>
#include <sys/stat.h>

// ============================================================
//  FILE I/O
// ============================================================
std::string loadFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) return "";

    std::ostringstream ss;
    ss << file.rdbuf();
    file.close();
    return ss.str();
}

bool saveFile(const std::string& filePath, const std::string& content) {
    std::ofstream file(filePath);
    if (!file.is_open()) return false;
    file << content;
    file.close();
    return true;
}

bool catatKeIndex(const std::string& savePath, const std::string& namaFile) {
    std::string indexPath = savePath + INDEX_FILE;
    std::ofstream file(indexPath, std::ios::app);
    if (!file.is_open()) return false;
    file << namaFile << "\n";
    file.close();
    return true;
}

std::string* loadIndex(const std::string& savePath, int& count) {
    count = 0;
    std::string indexPath = savePath + INDEX_FILE;

    std::ifstream file(indexPath);
    if (!file.is_open()) return nullptr;

    // Hitung jumlah baris tidak kosong
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) count++;
    }
    if (count == 0) { file.close(); return nullptr; }

    // Baca ulang dari awal
    file.clear();
    file.seekg(0);

    std::string* result = new std::string[count];
    int i = 0;
    while (std::getline(file, line)) {
        if (!line.empty()) result[i++] = line;
    }
    file.close();
    return result;
}

// ============================================================
//  CONFIG
// ============================================================
std::string loadConfig() {
    std::ifstream file(CONFIG_FILE);
    if (!file.is_open()) return DEFAULT_SAVE_PATH;

    std::string line;
    if (std::getline(file, line) && !line.empty()) {
        file.close();
        // Pastikan diakhiri slash
        if (line.back() != '/' && line.back() != '\\')
            line += '/';
        return line;
    }

    file.close();
    return DEFAULT_SAVE_PATH;
}

bool saveConfig(const std::string& savePath) {
    std::ofstream file(CONFIG_FILE);
    if (!file.is_open()) return false;
    file << savePath << "\n";
    file.close();
    return true;
}

// ============================================================
//  UTILITY
// ============================================================
void ensureSaveDir(const std::string& savePath) {
#if defined(_WIN32)
    mkdir(savePath.c_str());
#else
    mkdir(savePath.c_str(), 0755);
#endif
}
