#include "undoredo.h"

// ============================================================
//  STACK
// ============================================================
Stack::Stack() : _top(nullptr) {}

Stack::~Stack() { clear(); }

void Stack::push(const std::string& text) {
    StackNode* node = new StackNode(text);
    node->next = _top;
    _top = node;
}

void Stack::pop() {
    if (!_top) return;
    StackNode* tmp = _top;
    _top = _top->next;
    delete tmp;
}

std::string Stack::top() const {
    if (!_top) return "";
    return _top->text;
}

bool Stack::isEmpty() const { return _top == nullptr; }

void Stack::clear() {
    while (!isEmpty()) pop();
}

// ============================================================
//  FUNGSI UNDO
// ============================================================
std::string undoAction(std::string& currentText,
                       Stack& undoStack,
                       Stack& redoStack)
{
    if (undoStack.isEmpty()) return currentText;

    // Simpan teks saat ini ke redoStack
    redoStack.push(currentText);

    // Ambil teks dari undoStack
    currentText = undoStack.top();
    undoStack.pop();

    return currentText;
}

// ============================================================
//  FUNGSI REDO
// ============================================================
std::string redoAction(std::string& currentText,
                       Stack& undoStack,
                       Stack& redoStack)
{
    if (redoStack.isEmpty()) return currentText;

    // Simpan teks saat ini ke undoStack
    undoStack.push(currentText);

    // Ambil teks dari redoStack
    currentText = redoStack.top();
    redoStack.pop();

    return currentText;
}
