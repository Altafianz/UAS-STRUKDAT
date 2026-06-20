#include "undoredo.h"
using namespace std;

// ============================================================
//  STACK
// ============================================================
void initStack(Stack& s) {
    s.top = NULL;
}

void push(Stack& s, const string& text) {
    StackNode* baru = new StackNode();
    baru->text = text;
    baru->next = s.top;
    s.top = baru;
}

void pop(Stack& s) {
    if (s.top == NULL) return;
    StackNode* hapus = s.top;
    s.top = s.top->next;
    delete hapus;
}

string peek(const Stack& s) {
    if (s.top == NULL) return "";
    return s.top->text;
}

bool isStackEmpty(const Stack& s) {
    return s.top == NULL;
}

void clearStack(Stack& s) {
    while (s.top != NULL) {
        pop(s);
    }
}

// ============================================================
//  UNDO
// ============================================================
string undoAction(string& currentText, Stack& undoStack, Stack& redoStack) {
    if (isStackEmpty(undoStack)) return currentText;

    // Simpan teks saat ini ke redoStack
    push(redoStack, currentText);

    // Ambil teks dari undoStack
    currentText = peek(undoStack);
    pop(undoStack);

    return currentText;
}

// ============================================================
//  REDO
// ============================================================
string redoAction(string& currentText, Stack& undoStack, Stack& redoStack) {
    if (isStackEmpty(redoStack)) return currentText;

    // Simpan teks saat ini ke undoStack
    push(undoStack, currentText);

    // Ambil teks dari redoStack
    currentText = peek(redoStack);
    pop(redoStack);

    return currentText;
}
