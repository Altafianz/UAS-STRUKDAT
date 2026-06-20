#ifndef UNDOREDO_H
#define UNDOREDO_H

#include <string>
using namespace std;

// ============================================================
//  STACK NODE
// ============================================================
struct StackNode {
    string     text;
    StackNode* next;
};

// ============================================================
//  STACK — untuk undoStack dan redoStack
//  Menggunakan linked list (efisien untuk string panjang)
// ============================================================
struct Stack {
    StackNode* top;
};

// Inisialisasi stack (wajib dipanggil sebelum dipakai)
void initStack(Stack& s);

// Push teks ke stack
void push(Stack& s, const string& text);

// Pop node teratas
void pop(Stack& s);

// Ambil teks teratas tanpa menghapus
string peek(const Stack& s);

// Cek apakah stack kosong
bool isStackEmpty(const Stack& s);

// Kosongkan seluruh stack
void clearStack(Stack& s);

// ============================================================
//  FUNGSI UNDO & REDO
//  Return: teks yang harus di-set ke QTextEdit.
//          Jika stack kosong, kembalikan currentText apa adanya.
// ============================================================
string undoAction(string& currentText, Stack& undoStack, Stack& redoStack);
string redoAction(string& currentText, Stack& undoStack, Stack& redoStack);

#endif // UNDOREDO_H
