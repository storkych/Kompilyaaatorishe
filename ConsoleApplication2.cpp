/*
Чтобы воспользоваться менеджером памяти:
Добавляем файл allocator.cpp в файлы ресурсов проекта
Прописываем #include "allocator.cpp"
Прописываем allocatorh::Heap heap (максимальный размер в байтах, сколько нам понадобится на 1 объект)
Для выделения памяти под объект - void* block1 = heap.allocate(размер объекта)
В block1 будет записан указатель на начало блока в памяти
Для очищения памяти - heap.free(block1)
*/

// Добавляем элементы, выбираем и удаляем элементы, а затем снова добавляем и удаляем элементы для каждой из структур данных.
// Логирование состояний позволяет отслеживать изменения в каждой структуре данных, а аллокатор управляет динамической памятью.

#include "allocator.cpp"
#include <iostream>

class List {
protected:
    allocatorh::Heap& heap;

public:
    List(allocatorh::Heap& _heap) : heap(_heap), front(nullptr) {}
    virtual void push(void* data) = 0;
    virtual void* pop() = 0;
    virtual ~List() {}

    void* front;
    virtual void logState() {
        std::cout << "    State: " << (front != nullptr ? "Non-empty" : "Empty") << std::endl;
        heap.printMemoryUsage();
    }
};

class Stack : public List {
private:
    size_t elementSize;

public:
    Stack(allocatorh::Heap& _heap, size_t _elementSize) : List(_heap), elementSize(_elementSize) {}

    void push(void* data) override {
        front = heap.allocate(elementSize);
        std::memcpy(front, data, elementSize);

    }

    void* pop() override {
        if (front != nullptr) {
            void* data = heap.allocate(elementSize);
            std::memcpy(data, front, elementSize);
            heap.free(front);
            front = nullptr;

            return data;
        }
        return nullptr;
    }
};

class Queue : public List {
private:
    size_t elementSize;
    void* rear;

public:
    Queue(allocatorh::Heap& _heap, size_t _elementSize) : List(_heap), elementSize(_elementSize), rear(nullptr) {}

    void push(void* data) override {
        if (rear == nullptr) {
            rear = heap.allocate(elementSize);
            front = rear;
        }
        else {
            rear = heap.allocate(elementSize);
        }
        std::memcpy(rear, data, elementSize);

    }

    void* pop() override {
        if (front != nullptr) {
            void* data = heap.allocate(elementSize);
            std::memcpy(data, front, elementSize);
            heap.free(front);
            if (front != rear) {
                front = static_cast<char*>(front) + elementSize;
            }
            else {
                front = rear = nullptr;
            }

            return data;
        }
        return nullptr;
    }
};

class Deque : public List {
private:
    size_t elementSize;
    void* rear;

public:
    Deque(allocatorh::Heap& _heap, size_t _elementSize) : List(_heap), elementSize(_elementSize), rear(nullptr) {}

    void push(void* data) override {
        if (front == nullptr) {
            front = heap.allocate(elementSize);
            rear = front;
        }
        else {
            front = heap.allocate(elementSize);
        }
        std::memcpy(front, data, elementSize);

    }

    void* pop() override {
        if (rear != nullptr) {
            void* data = heap.allocate(elementSize);
            std::memcpy(data, rear, elementSize);
            heap.free(rear);
            if (front != rear) {
                rear = static_cast<char*>(rear) - elementSize;
            }
            else {
                front = rear = nullptr;
            }

            return data;
        }
        return nullptr;
    }
};


int main() {
    setlocale(LC_ALL, "Russian");
    allocatorh::Heap heap(1024);

    Stack stack(heap, sizeof(int));
    Queue queue(heap, sizeof(float));
    Deque deque(heap, sizeof(double));

    int stackData[] = { 1, 2, 3 };
    for (int i = 0; i < 3; ++i) {
        stack.push(&stackData[i]);
    }
    stack.logState();
    float queueData[] = { 4.0, 5.0, 6.0 };
    for (int i = 0; i < 3; ++i) {
        queue.push(&queueData[i]);
    }
    queue.logState();
    double dequeData[] = { 7.0, 8.0, 9.0 };
    for (int i = 0; i < 3; ++i) {
        deque.push(&dequeData[i]);
    }
    deque.logState();

    int* poppedFromStack = static_cast<int*>(stack.pop());
    std::cout << "Popped from Stack: " << *poppedFromStack << std::endl;
    stack.logState();

    float* poppedFromQueue = static_cast<float*>(queue.pop());
    std::cout << "Popped from Queue: " << *poppedFromQueue << std::endl;
    stack.logState();

    double* poppedFromDeque = static_cast<double*>(deque.pop());
    std::cout << "Popped from Deque: " << *poppedFromDeque << std::endl;
    stack.logState();

    std::cout << "Добавили в стэк 3";
    int newStackData[] = { 10, 11, 12 };
    for (int i = 0; i < 3; ++i) {
        stack.push(&newStackData[i]);
    }
    stack.logState();
    std::cout << "Добавили в очередь 3";
    float newQueueData[] = { 13.0, 14.0, 15.0 };
    for (int i = 0; i < 3; ++i) {
        queue.push(&newQueueData[i]);
    }
    stack.logState();
    std::cout << "Добавили в дэк 3";
    double newDequeData[] = { 16.0, 17.0, 18.0 };
    for (int i = 0; i < 3; ++i) {
        deque.push(&newDequeData[i]);
    }
    stack.logState();

    for (int i = 0; i < 3; ++i) {
        stack.pop();
    }

    for (int i = 0; i < 3; ++i) {
        queue.pop();
    }

    for (int i = 0; i < 3; ++i) {
        deque.pop();
    }

    //heap.printMemoryUsage();

    return 0;
}
