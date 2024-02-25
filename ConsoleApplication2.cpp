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

// Абстрактный класс для списка - базовый для стека, очереди и дека. Далее три класса-наследника от этого абстрактного класса: `Stack`, `Queue` и `Deque`
class List {
public:
    virtual void push(void* data) = 0;  // Добавление элемента
    virtual void* pop() = 0;            // Выборка и удаление элемента
    virtual void logState() const = 0;  // Логирование состояния структуры
    virtual ~List() {}
};

// Класс для стека
class Stack : public List {
private:
    allocatorh::Heap& heap;  // Ссылка на объект аллокатора
    size_t elementSize;      // Размер элемента
    void* top;               // Указатель на вершину стека

public:
    Stack(allocatorh::Heap& _heap, size_t _elementSize) : heap(_heap), elementSize(_elementSize), top(nullptr) {}

    void push(void* data) override {
        top = heap.allocate(elementSize);
        std::memcpy(top, data, elementSize);
        logState();
    }

    void* pop() override {
        if (top != nullptr) {
            void* data = heap.allocate(elementSize);
            std::memcpy(data, top, elementSize);
            heap.free(top);
            top = nullptr;
            logState();
            return data;
        }
        return nullptr;
    }

    void logState() const override {
        std::cout << "Stack State: " << (top != nullptr ? "Non-empty" : "Empty") << std::endl;
    }
};

// Класс для очереди
class Queue : public List {
private:
    allocatorh::Heap& heap;  // Ссылка на объект аллокатора
    size_t elementSize;      // Размер элемента
    void* front;             // Указатель на начало очереди
    void* rear;              // Указатель на конец очереди

public:
    Queue(allocatorh::Heap& _heap, size_t _elementSize) : heap(_heap), elementSize(_elementSize), front(nullptr), rear(nullptr) {}

    void push(void* data) override {
        if (rear == nullptr) {
            rear = heap.allocate(elementSize);
            front = rear;
        }
        else {
            rear = heap.allocate(elementSize);
        }
        std::memcpy(rear, data, elementSize);
        logState();
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
            logState();
            return data;
        }
        return nullptr;
    }

    void logState() const override {
        std::cout << "Queue State: " << (front != nullptr ? "Non-empty" : "Empty") << std::endl;
    }
};

// Класс для дека
class Deque : public List {
private:
    allocatorh::Heap& heap;  // Ссылка на объект аллокатора
    size_t elementSize;      // Размер элемента
    void* front;             // Указатель на начало дека
    void* rear;              // Указатель на конец дека

public:
    Deque(allocatorh::Heap& _heap, size_t _elementSize) : heap(_heap), elementSize(_elementSize), front(nullptr), rear(nullptr) {}

    void push(void* data) override {
        if (front == nullptr) {
            front = heap.allocate(elementSize);
            rear = front;
        }
        else {
            front = heap.allocate(elementSize);
        }
        std::memcpy(front, data, elementSize);
        logState();
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
            logState();
            return data;
        }
        return nullptr;
    }

    void logState() const override {
        std::cout << "Deque State: " << (front != nullptr ? "Non-empty" : "Empty") << std::endl;
    }
};

int main() {
    setlocale(LC_ALL, "Russian");
    allocatorh::Heap heap(1024); // Размер выделенной памяти 1024 байта

    // Демонстрационный режим
    Stack stack(heap, sizeof(int));
    Queue queue(heap, sizeof(float));
    Deque deque(heap, sizeof(double));

    // Добавление элементов
    int stackData[] = { 1, 2, 3 };
    for (int i = 0; i < 3; ++i) {
        stack.push(&stackData[i]);
    }

    float queueData[] = { 4.0, 5.0, 6.0 };
    for (int i = 0; i < 3; ++i) {
        queue.push(&queueData[i]);
    }

    /*
    //double dequeData[] = { 7.0, 8.0, 9.0 };
    //for (int i = 0; i < 3; ++i) {
    //    deque.push(&dequeData[i]);
    //}

    // Логирование состояний после добавления
    stack.logState();
    queue.logState();
    deque.logState();

    // Выборка и удаление элементов
    int* poppedFromStack = static_cast<int*>(stack.pop());
    std::cout << "Popped from Stack: " << *poppedFromStack << std::endl;

    float* poppedFromQueue = static_cast<float*>(queue.pop());
    std::cout << "Popped from Queue: " << *poppedFromQueue << std::endl;

    //double* poppedFromDeque = static_cast<double*>(deque.pop());
    //std::cout << "Popped from Deque: " << *poppedFromDeque << std::endl;

    // Логирование состояний после удаления
    stack.logState();
    queue.logState();
    deque.logState();

    //Добавление элементов после удаления
    //int newStackData[] = { 10, 11, 12 };
    //for (int i = 0; i < 3; ++i) {
    //    stack.push(&newStackData[i]);
    //}
    //float newQueueData[] = { 13.0, 14.0, 15.0 };
    //for (int i = 0; i < 3; ++i) {
    //    queue.push(&newQueueData[i]);
    //}

    double newDequeData[] = { 16.0, 17.0, 18.0 };
    for (int i = 0; i < 3; ++i) {
        deque.push(&newDequeData[i]);
    }

    // Логирование состояний после добавления
    stack.logState();
    queue.logState();
    deque.logState();

    // Очистка памяти
    heap.printMemoryUsage();

    // Удаление всех элементов из стека
    for (int i = 0; i < 3; ++i) {
        stack.pop();
    }

    // Удаление всех элементов из очереди
    for (int i = 0; i < 3; ++i) {
        queue.pop();
    }

    // Удаление всех элементов из дека
    for (int i = 0; i < 3; ++i) {
        deque.pop();
    }

    // Логирование состояний после удаления всех элементов
    stack.logState();
    queue.logState();
    deque.logState();

    // Очистка памяти после удаления всех элементов
    heap.printMemoryUsage();

    return 0;
    */
}

/*
Демонстрационный режим - задание 1

int main()
{
    allocatorh::Heap heap(1024); // Выделение памяти размером 1024

    setlocale(LC_ALL, "Russian");
    void* block1 = heap.allocate(100); // Выделение блока памяти размером 100
    void* block2 = heap.allocate(200); // Выделение блока памяти размером 200
    void* block3 = heap.allocate(300); // Выделение блока памяти размером 300
    void* block4 = heap.allocate(400); // Выделение блока памяти размером 400
    void* block5 = heap.allocate(500); // Выделение блока памяти размером 500

    heap.printMemoryUsage(); // Вывод информации о памяти

    heap.free(block2); // Освобождение памяти для блока 2
    heap.free(block4); // Освобождение памяти для блока 4
    heap.free(block3); // Освобождение памяти для блока 3

    heap.printMemoryUsage(); // Вывод информации о памяти
    void* block6 = heap.allocate(1200); // Выделение блока памяти размером 600 (запрос памяти чуть большего размера, чем можно выделить для одного сегмента)

    void* block7 = heap.allocate(600); // Выделение блока памяти размером 300
    void* block8 = heap.allocate(600); // Выделение блока памяти размером 300
    void* block9 = heap.allocate(600); // Выделение блока памяти размером 300
    heap.free(block9); // Освобождение памяти для блока 3


    heap.printMemoryUsage(); // Вывод информации о памяти
}
*/