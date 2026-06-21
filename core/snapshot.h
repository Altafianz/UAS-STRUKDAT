#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include <string>
#include "undoredo.h"
using namespace std;

// ============================================================
//  KONSTANTA
// ============================================================
const int MAX_HISTORY = 50;

// ============================================================
//  NODE — Doubly Linked List (historyList)
// ============================================================
struct DLLNode {
    string   text;
    DLLNode* prev;
    DLLNode* next;
};

// ============================================================
//  DOUBLY LINKED LIST — historyList
// ============================================================
struct HistoryList {
    DLLNode* head;
    DLLNode* tail;
    DLLNode* current;
    int      size;
};

// Inisialisasi historyList
void initHistoryList(HistoryList& list);

// Tambah snapshot baru di akhir, return pointer node-nya
DLLNode* appendHistory(HistoryList& list, const string& text);

// Hapus node tertentu (saat queue overflow)
void removeNode(HistoryList& list, DLLNode* node);

// Hapus semua node setelah node ini (saat ada aksi baru setelah undo)
void removeAfter(HistoryList& list, DLLNode* node);

// Kosongkan seluruh list
void clearHistoryList(HistoryList& list);

// Geser pointer current mundur / maju satu langkah
void movePrev(HistoryList& list);
void moveNext(HistoryList& list);

// ============================================================
//  NODE — Queue
// ============================================================
struct QueueNode {
    DLLNode*   ref;   // referensi ke DLLNode yang sesuai
    QueueNode* next;
};

// ============================================================
//  BOUNDED QUEUE — historyQueue, batas MAX_HISTORY
// ============================================================
struct BoundedQueue {
    QueueNode* front;
    QueueNode* rear;
    int        size;
};

// Inisialisasi queue
void initQueue(BoundedQueue& q);

// Enqueue snapshot baru.
// Return: pointer DLLNode yang harus dihapus dari historyList,
//         atau NULL jika queue belum penuh.
DLLNode* enqueue(BoundedQueue& q, DLLNode* node);

// Kosongkan seluruh queue
void clearQueue(BoundedQueue& q);

// ============================================================
//  FUNGSI SNAPSHOT
// ============================================================

// Dipanggil setiap kali teks berubah dan karakter terakhir spasi
void saveSnapshot(string& currentText,
                  Stack& undoStack,
                  Stack& redoStack,
                  HistoryList& historyList,
                  BoundedQueue& historyQueue,
                  const string& newText);

// Lompat langsung ke snapshot yang dipilih dari dropdown
// Return: teks yang harus di-set ke QTextEdit
string jumpToSnapshot(string& currentText,
                      Stack& undoStack,
                      Stack& redoStack,
                      HistoryList& historyList,
                      DLLNode* targetNode);

#endif // SNAPSHOT_H
