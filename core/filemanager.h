#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>

// ============================================================
//  KONSTANTA PATH
// ============================================================
const std::string DEFAULT_SAVE_PATH = "./saves/";
const std::string CONFIG_FILE       = "config.txt";
const std::string INDEX_FILE        = "index.txt";

// ============================================================
//  FILE I/O
// ============================================================

// Baca isi file teks, kembalikan sebagai string.
// Kembalikan string kosong jika file tidak ditemukan / gagal dibuka.
std::string loadFile(const std::string& filePath);

// Tulis content ke filePath (overwrite).
// Kembalikan true jika berhasil.
bool saveFile(const std::string& filePath, const std::string& content);

// Tambahkan namaFile ke index.txt di dalam savePath (mode append).
// Kembalikan true jika berhasil.
bool catatKeIndex(const std::string& savePath, const std::string& namaFile);

// Baca seluruh isi index.txt di dalam savePath.
// Kembalikan array of string; jumlah elemen diisi ke 'count'.
// Kembalikan nullptr jika kosong atau tidak ditemukan.
// PERHATIAN: caller wajib delete[] hasil kembalian.
std::string* loadIndex(const std::string& savePath, int& count);

// ============================================================
//  CONFIG
// ============================================================

// Baca config.txt dan kembalikan savePath yang tersimpan.
// Jika tidak ada / kosong, kembalikan DEFAULT_SAVE_PATH.
std::string loadConfig();

// Tulis savePath ke config.txt.
// Kembalikan true jika berhasil.
bool saveConfig(const std::string& savePath);

// ============================================================
//  UTILITY
// ============================================================

// Buat folder savePath jika belum ada (cross-platform).
void ensureSaveDir(const std::string& savePath);

#endif // FILEMANAGER_H
