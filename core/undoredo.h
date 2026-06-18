#ifndef UNDOREDO_H
#define UNDOREDO_H

#include <string>

// ============================================================
//  STACK NODE
// ============================================================
struct StackNode {
    std::string text;
    StackNode*  next;

    StackNode(const std::string& t) : text(t), next(nullptr) {}
};

// ============================================================
//  STACK — dipakai untuk undoStack dan redoStack
// ============================================================
class Stack {
public:
    Stack();
    ~Stack();

    void        push(const std::string& text);
    void        pop();
    std::string top() const;
    bool        isEmpty() const;
    void        clear();

private:
    StackNode* _top;
};

// ============================================================
//  FUNGSI UNDO & REDO
//  Dipanggil dari EditorWindow saat tombol undo/redo diklik.
//  Parameter:
//    currentText  — teks yang sedang tampil di QTextEdit
//    undoStack    — stack undo milik editor
//    redoStack    — stack redo milik editor
//  Return value:
//    string teks yang harus di-set ke QTextEdit.
//    Jika stack kosong, kembalikan currentText apa adanya.
// ============================================================
std::string undoAction(std::string& currentText,
                       Stack& undoStack,
                       Stack& redoStack);

std::string redoAction(std::string& currentText,
                       Stack& undoStack,
                       Stack& redoStack);

#endif // UNDOREDO_H
