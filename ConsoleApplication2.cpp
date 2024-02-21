/*
Чтобы воспользоваться менеджером памяти:
Добавляем файл allocator.cpp в файлы ресурсов проекта
Прописываем #include "allocator.cpp"
Прописываем allocatorh::Heap heap (максимальный размер в байтах, сколько нам понадобится на 1 объект)
Для выделения памяти под объект - void* block1 = heap.allocate(размер объекта)
В block1 будет записан указатель на начало блока в памяти
Для очищения памяти - heap.free(block1)
*/

#include <iostream>
#include "allocator.cpp"

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