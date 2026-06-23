#include "snapshot.h"
using namespace std;

//  DOUBLY LINKED LIST
void initHistoryList(HistoryList &list)
{
    list.head = NULL;
    list.tail = NULL;
    list.current = NULL;
    list.size = 0;
}

DLLNode *appendHistory(HistoryList &list, const string &text)
{
    DLLNode *baru = new DLLNode();
    baru->text = text;
    baru->prev = NULL;
    baru->next = NULL;

    if (list.tail == NULL)
    {
        list.head = list.tail = baru;
    }
    else
    {
        baru->prev = list.tail;
        list.tail->next = baru;
        list.tail = baru;
    }

    list.current = baru;
    list.size++;
    return baru;
}

void removeNode(HistoryList &list, DLLNode *node)
{
    if (node == NULL)
        return;

    if (node->prev != NULL)
        node->prev->next = node->next;
    else
        list.head = node->next;

    if (node->next != NULL)
        node->next->prev = node->prev;
    else
        list.tail = node->prev;

    if (list.current == node)
        list.current = (node->next != NULL) ? node->next : node->prev;

    delete node;
    list.size--;
}

void removeAfter(HistoryList &list, DLLNode *node)
{
    if (node == NULL)
        return;
    DLLNode *cur = node->next;
    while (cur != NULL)
    {
        DLLNode *tmp = cur->next;
        delete cur;
        list.size--;
        cur = tmp;
    }
    node->next = NULL;
    list.tail = node;
}

//  bounded queue
void initQueue(BoundedQueue &q)
{
    q.front = NULL;
    q.rear = NULL;
    q.size = 0;
}

DLLNode *enqueue(BoundedQueue &q, DLLNode *node)
{
    QueueNode *baru = new QueueNode();
    baru->ref = node;
    baru->next = NULL;

    if (q.rear == NULL)
    {
        q.front = q.rear = baru;
    }
    else
    {
        q.rear->next = baru;
        q.rear = baru;
    }
    q.size++;

    // Jika melebihi batas, evict node paling lama
    if (q.size > MAX_HISTORY)
    {
        QueueNode *hapus = q.front;
        DLLNode *evicted = hapus->ref;
        q.front = q.front->next;
        if (q.front == NULL)
            q.rear = NULL;
        delete hapus;
        q.size--;
        return evicted; // caller wajib hapus dari historyList
    }

    return NULL;
}

//  save snapshot
void saveSnapshot(string &currentText,Stack &undoStack,Stack &redoStack,HistoryList &historyList, BoundedQueue &historyQueue,const string &newText)
{
    // Jika ada node setelah current, hapus dulu (aksi baru setelah undo)
    if (historyList.current != NULL && historyList.current != historyList.tail)
    {
        removeAfter(historyList, historyList.current);
    }

    // Append snapshot baru ke DLL
    DLLNode *newNode = appendHistory(historyList, newText);

    // Masukkan ke queue; jika ada yang dievict, hapus dari DLL juga
    DLLNode *evicted = enqueue(historyQueue, newNode);
    if (evicted != NULL)
    {
        removeNode(historyList, evicted);
    }

    // Push teks lama ke undoStack
    push(undoStack, currentText);

    // Kosongkan redoStack, ada aksi baru
    clearStack(redoStack);

    // Update teks aktif
    currentText = newText;
}

//  JUMP TO SNAPSHOT
string jumpToSnapshot(string &currentText, Stack &undoStack, Stack &redoStack, HistoryList &historyList, DLLNode *targetNode)
{
    if (targetNode == NULL)
        return currentText;

    clearStack(undoStack);
    clearStack(redoStack);

    // Hitung jumlah node sebelum target
    int count = 0;
    DLLNode *cur = historyList.head;
    while (cur != NULL && cur != targetNode)
    {
        count++;
        cur = cur->next;
    }

    // Push ke undoStack (pakai array sementara agar urutan benar)
    if (count > 0)
    {
        string *tmp = new string[count];
        cur = historyList.head;
        for (int i = 0; i < count; i++)
        {
            tmp[i] = cur->text;
            cur = cur->next;
        }
        for (int i = 0; i < count; i++)
        {
            push(undoStack, tmp[i]);
        }
        delete[] tmp;
    }

    // Hitung jumlah node setelah target
    int rcount = 0;
    DLLNode *r = targetNode->next;
    DLLNode *rCur = r;
    while (rCur != NULL)
    {
        rcount++;
        rCur = rCur->next;
    }

    // Push ke redoStack (terbalik agar top = terdekat ke target)
    if (rcount > 0)
    {
        string *rtmp = new string[rcount];
        rCur = r;
        for (int i = 0; i < rcount; i++)
        {
            rtmp[i] = rCur->text;
            rCur = rCur->next;
        }
        for (int i = rcount - 1; i >= 0; i--)
        {
            push(redoStack, rtmp[i]);
        }
        delete[] rtmp;
    }

    historyList.current = targetNode;
    currentText = targetNode->text;
    return currentText;
}
