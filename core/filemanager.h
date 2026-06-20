#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>
using namespace std;

// ============================================================
//  KONSTANTA PATH
// ============================================================
const string DEFAULT_SAVE_PATH = "./saves/";
const string CONFIG_FILE       = "config.txt";
const string INDEX_FILE        = "index.txt";

// ============================================================
//  FILE I/O
// ============================================================

// Baca isi file teks, kembalikan sebagai string
// Kembalikan string kosong jika gagal
string loadFile(const string& filePath);

// Tulis content ke filePath (overwrite)
// Kembalikan true jika berhasil
bool saveFile(const string& filePath, const string& content);

// Tambahkan namaFile ke index.txt di dalam savePath (mode append)
// Kembalikan true jika berhasil
bool catatKeIndex(const string& savePath, const string& namaFile);

// Baca seluruh isi index.txt di dalam savePath
// Kembalikan array of string; jumlah elemen diisi ke 'count'
// Kembalikan NULL jika kosong atau tidak ditemukan
// PERHATIAN: caller wajib delete[] hasil kembalian
string* loadIndex(const string& savePath, int& count);

// ============================================================
//  CONFIG
// ============================================================

// Baca config.txt, kembalikan savePath yang tersimpan
// Jika tidak ada / kosong, kembalikan DEFAULT_SAVE_PATH
string loadConfig();

// Tulis savePath ke config.txt
// Kembalikan true jika berhasil
bool saveConfig(const string& savePath);

// ============================================================
//  UTILITY
// ============================================================

// Buat folder savePath jika belum ada (cross-platform)
void ensureSaveDir(const string& savePath);

#endif // FILEMANAGER_H
