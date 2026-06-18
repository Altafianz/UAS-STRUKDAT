#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include <string>
#include "undoredo.h"   // butuh Stack untuk jumpToSnapshot

// ============================================================
//  KONSTANTA
// ============================================================
const int MAX_HISTORY = 100;

// ============================================================
//  NODE — Doubly Linked List
// ============================================================
struct DLLNode {
    std::string text;
    DLLNode*    prev;
    DLLNode*    next;

    DLLNode(const std::string& t) : text(t), prev(nullptr), next(nullptr) {}
};

// ============================================================
//  DOUBLY LINKED LIST — historyList
//  Menyimpan semua snapshot teks secara berurutan.
//  Pointer current menunjuk posisi aktif saat ini.
// ============================================================
class HistoryList {
public:
    HistoryList();
    ~HistoryList();

    // Tambah snapshot baru di akhir list, return pointer node-nya
    DLLNode* append(const std::string& text);

    // Hapus node tertentu (dipanggil saat queue overflow)
    void removeNode(DLLNode* node);

    // Hapus semua node setelah node ini (dipanggil saat ada aksi baru setelah undo)
    void removeAfter(DLLNode* node);

    void clear();

    DLLNode* head()    const;
    DLLNode* tail()    const;
    DLLNode* current() const;
    void     setCurrent(DLLNode* node);

    bool movePrev();   // geser current mundur satu langkah
    bool moveNext();   // geser current maju satu langkah

    int size() const;

private:
    DLLNode* _head;
    DLLNode* _tail;
    DLLNode* _current;
    int      _size;
};

// ============================================================
//  QUEUE NODE — menyimpan referensi ke DLLNode
// ============================================================
struct QueueNode {
    DLLNode*   ref;
    QueueNode* next;

    QueueNode(DLLNode* r) : ref(r), next(nullptr) {}
};

// ============================================================
//  BOUNDED QUEUE — historyQueue, batas MAX_HISTORY
//  Saat penuh, node paling lama di-evict dan harus dihapus
//  dari HistoryList oleh caller.
// ============================================================
class BoundedQueue {
public:
    BoundedQueue();
    ~BoundedQueue();

    // Enqueue snapshot baru.
    // Return: pointer DLLNode yang harus dihapus dari HistoryList,
    //         atau nullptr jika queue belum penuh.
    DLLNode* enqueue(DLLNode* node);

    void clear();
    int  size() const;

private:
    QueueNode* _front;
    QueueNode* _rear;
    int        _size;
};

// ============================================================
//  FUNGSI SNAPSHOT
// ============================================================

// Dipanggil dari EditorWindow setiap kali teks berubah
// dan karakter terakhir adalah spasi.
// newText = isi QTextEdit saat ini.
void saveSnapshot(std::string&  currentText,
                  Stack&        undoStack,
                  Stack&        redoStack,
                  HistoryList&  historyList,
                  BoundedQueue& historyQueue,
                  const std::string& newText);

// Lompat langsung ke snapshot yang dipilih user dari dropdown.
// Setelah ini undoStack berisi semua node sebelum target,
// redoStack berisi semua node setelah target.
// Return: teks yang harus di-set ke QTextEdit.
std::string jumpToSnapshot(std::string&  currentText,
                           Stack&        undoStack,
                           Stack&        redoStack,
                           HistoryList&  historyList,
                           DLLNode*      targetNode);

#endif // SNAPSHOT_H
