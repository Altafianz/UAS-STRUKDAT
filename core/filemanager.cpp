#include "filemanager.h"

#include <fstream>
#include <sstream>
#include <sys/stat.h>
using namespace std;

// ============================================================
//  FILE I/O
// ============================================================
string loadFile(const string& filePath) {
    ifstream file(filePath);
    if (!file.is_open()) return "";

    ostringstream ss;
    ss << file.rdbuf();
    file.close();
    return ss.str();
}

bool saveFile(const string& filePath, const string& content) {
    ofstream file(filePath);
    if (!file.is_open()) return false;
    file << content;
    file.close();
    return true;
}

bool catatKeIndex(const string& savePath, const string& namaFile) {
    string indexPath = savePath + INDEX_FILE;
    ofstream file(indexPath, ios::app);
    if (!file.is_open()) return false;
    file << namaFile << "\n";
    file.close();
    return true;
}

string* loadIndex(const string& savePath, int& count) {
    count = 0;
    string indexPath = savePath + INDEX_FILE;

    ifstream file(indexPath);
    if (!file.is_open()) return NULL;

    // Hitung jumlah baris tidak kosong
    string line;
    while (getline(file, line)) {
        if (!line.empty()) count++;
    }
    if (count == 0) { file.close(); return NULL; }

    // Baca ulang dari awal
    file.clear();
    file.seekg(0);

    string* result = new string[count];
    int i = 0;
    while (getline(file, line)) {
        if (!line.empty()) result[i++] = line;
    }
    file.close();
    return result;
}

// ============================================================
//  CONFIG
// ============================================================
string loadConfig() {
    ifstream file(CONFIG_FILE);
    if (!file.is_open()) return DEFAULT_SAVE_PATH;

    string line;
    if (getline(file, line) && !line.empty()) {
        file.close();
        // Pastikan diakhiri slash
        if (line.back() != '/' && line.back() != '\\')
            line += '/';
        return line;
    }

    file.close();
    return DEFAULT_SAVE_PATH;
}

bool saveConfig(const string& savePath) {
    ofstream file(CONFIG_FILE);
    if (!file.is_open()) return false;
    file << savePath << "\n";
    file.close();
    return true;
}

// ============================================================
//  UTILITY
// ============================================================
void ensureSaveDir(const string& savePath) {
#if defined(_WIN32)
    mkdir(savePath.c_str());
#else
    mkdir(savePath.c_str(), 0755);
#endif
}
