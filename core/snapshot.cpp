#include "snapshot.h"

// ============================================================
//  HISTORY LIST
// ============================================================
HistoryList::HistoryList()
    : _head(nullptr), _tail(nullptr), _current(nullptr), _size(0) {}

HistoryList::~HistoryList() { clear(); }

DLLNode* HistoryList::append(const std::string& text) {
    DLLNode* node = new DLLNode(text);
    if (!_tail) {
        _head = _tail = node;
    } else {
        node->prev  = _tail;
        _tail->next = node;
        _tail       = node;
    }
    _current = node;
    _size++;
    return node;
}

void HistoryList::removeNode(DLLNode* node) {
    if (!node) return;

    if (node->prev) node->prev->next = node->next;
    else            _head = node->next;

    if (node->next) node->next->prev = node->prev;
    else            _tail = node->prev;

    if (_current == node)
        _current = node->next ? node->next : node->prev;

    delete node;
    _size--;
}

void HistoryList::removeAfter(DLLNode* node) {
    if (!node) return;
    DLLNode* cur = node->next;
    while (cur) {
        DLLNode* tmp = cur->next;
        delete cur;
        _size--;
        cur = tmp;
    }
    node->next = nullptr;
    _tail      = node;
}

void HistoryList::clear() {
    DLLNode* cur = _head;
    while (cur) {
        DLLNode* tmp = cur->next;
        delete cur;
        cur = tmp;
    }
    _head = _tail = _current = nullptr;
    _size = 0;
}

DLLNode* HistoryList::head()    const { return _head; }
DLLNode* HistoryList::tail()    const { return _tail; }
DLLNode* HistoryList::current() const { return _current; }
void     HistoryList::setCurrent(DLLNode* node) { _current = node; }
int      HistoryList::size()    const { return _size; }

bool HistoryList::movePrev() {
    if (_current && _current->prev) {
        _current = _current->prev;
        return true;
    }
    return false;
}

bool HistoryList::moveNext() {
    if (_current && _current->next) {
        _current = _current->next;
        return true;
    }
    return false;
}

// ============================================================
//  BOUNDED QUEUE
// ============================================================
BoundedQueue::BoundedQueue() : _front(nullptr), _rear(nullptr), _size(0) {}

BoundedQueue::~BoundedQueue() { clear(); }

DLLNode* BoundedQueue::enqueue(DLLNode* node) {
    QueueNode* qn = new QueueNode(node);

    if (!_rear) {
        _front = _rear = qn;
    } else {
        _rear->next = qn;
        _rear       = qn;
    }
    _size++;

    // Jika melebihi batas, evict node paling lama
    if (_size > MAX_HISTORY) {
        QueueNode* old     = _front;
        DLLNode*   evicted = old->ref;
        _front = _front->next;
        if (!_front) _rear = nullptr;
        delete old;
        _size--;
        return evicted;   // caller wajib hapus dari HistoryList
    }

    return nullptr;
}

void BoundedQueue::clear() {
    while (_front) {
        QueueNode* tmp = _front;
        _front = _front->next;
        delete tmp;
    }
    _rear  = nullptr;
    _size  = 0;
}

int BoundedQueue::size() const { return _size; }

// ============================================================
//  SAVE SNAPSHOT
// ============================================================
void saveSnapshot(std::string&  currentText,
                  Stack&        undoStack,
                  Stack&        redoStack,
                  HistoryList&  historyList,
                  BoundedQueue& historyQueue,
                  const std::string& newText)
{
    // Jika ada node setelah current (aksi baru setelah undo),
    // hapus semua node setelah current terlebih dahulu
    if (historyList.current() &&
        historyList.current() != historyList.tail()) {
        historyList.removeAfter(historyList.current());
    }

    // Append snapshot baru ke DLL
    DLLNode* newNode = historyList.append(newText);

    // Masukkan ke bounded queue; jika evict, hapus dari DLL juga
    DLLNode* evicted = historyQueue.enqueue(newNode);
    if (evicted) {
        historyList.removeNode(evicted);
    }

    // Push teks lama ke undoStack sebelum update currentText
    undoStack.push(currentText);

    // Kosongkan redoStack — ada aksi baru
    redoStack.clear();

    // Update teks aktif
    currentText = newText;
}

// ============================================================
//  JUMP TO SNAPSHOT
// ============================================================
std::string jumpToSnapshot(std::string&  currentText,
                           Stack&        undoStack,
                           Stack&        redoStack,
                           HistoryList&  historyList,
                           DLLNode*      targetNode)
{
    if (!targetNode) return currentText;

    undoStack.clear();
    redoStack.clear();

    // Hitung jumlah node sebelum target
    int count = 0;
    DLLNode* cur = historyList.head();
    while (cur && cur != targetNode) { count++; cur = cur->next; }

    // Kumpulkan teks node sebelum target ke array sementara,
    // lalu push ke undoStack agar top = node terdekat ke target
    if (count > 0) {
        std::string* tmp = new std::string[count];
        cur = historyList.head();
        for (int i = 0; i < count; i++) {
            tmp[i] = cur->text;
            cur    = cur->next;
        }
        for (int i = 0; i < count; i++) {
            undoStack.push(tmp[i]);
        }
        delete[] tmp;
    }

    // Kumpulkan teks node setelah target ke array sementara,
    // lalu push terbalik ke redoStack agar top = node terdekat ke target
    int rcount = 0;
    DLLNode* r = targetNode->next;
    DLLNode* rCur = r;
    while (rCur) { rcount++; rCur = rCur->next; }

    if (rcount > 0) {
        std::string* rtmp = new std::string[rcount];
        rCur = r;
        for (int i = 0; i < rcount; i++) {
            rtmp[i] = rCur->text;
            rCur    = rCur->next;
        }
        for (int i = rcount - 1; i >= 0; i--) {
            redoStack.push(rtmp[i]);
        }
        delete[] rtmp;
    }

    // Update pointer dan teks aktif
    historyList.setCurrent(targetNode);
    currentText = targetNode->text;

    return currentText;
}
