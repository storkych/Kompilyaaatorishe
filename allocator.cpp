#include <iostream>
#include <vector>

using namespace std;

namespace allocatorh {

    // Структура для хранения информации о каждом блоке памяти
    struct MemoryBlock {
        size_t size;  // Размер блока
        bool used;    // Статус блока (используется/не используется)
        size_t address; // Адрес верхнеуровневого сегмента
        // Адрес верхнеуровневого сегмента в этой программе - это смещение (в байтах) относительно начала памяти, 
        // зарезервированной транслятором, до верхнего блока памяти в векторе memoryBlocks.
    };

    // Представляет управление памятью
    extern class Heap {
    private:
        void* memory; // Указатель на память, зарезервированную транслятором (максимальный размер одного блока)
        size_t reservedSize; // Размер памяти, зарезервированный транслятором
        vector<MemoryBlock> memoryBlocks; // Вектор для хранения информации о каждом блоке памяти

    public:
        Heap(size_t size) {
            memory = allocateMemory(size); // Зарезервировать память размером size (максимальный размер одного блока)
            reservedSize = size;
        }

        ~Heap() {
            freeMemory(memory); // Освободить зарезервированную память
        }

        void* allocate(size_t size) {
            void* block = nullptr; // указатель на начало блока памяти

            // Поиск свободного блока памяти
            // Бежим по элементам вектора (auto - ключевое слово, используемое для определения типа итерируемых элементов)
            for (auto& memoryBlock : memoryBlocks) {
                // Если блок не используется и его размер >= нового 
                if (!memoryBlock.used && memoryBlock.size >= size) {
                    memoryBlock.used = true; // Пометить блок памяти как используемый
                    // В C++ есть ограничения для арифметических операций с указателями на тип void*. Такие указатели не совместимы с арифметическими операциями
                    // поэтому преобразовываем к char*
                    // начало памяти + смещение удалённого блока
                    block = static_cast<char*>(memory) + memoryBlock.address; // Вернуть указатель на начало блока памяти
                    break;
                }
            }

            // Если не удалось найти свободный блок, создаем новый блок памяти
            // Если не удалось найти свободных участков в памяти и размер меньше чем максимальный размер для одного блока
            if (block == nullptr && size <= reservedSize) {
                MemoryBlock newBlock = { size, true, memoryBlocks.empty() ? 0 : (memoryBlocks.back().address + memoryBlocks.back().size) }; // Создать новый блок памяти
                block = static_cast<char*>(memory) + newBlock.address; // Вернуть указатель на начало нового блока памяти
                memoryBlocks.push_back(newBlock); // Добавить информацию о новом блоке в вектор memoryBlocks
            }
            if (size > reservedSize)
            {
                cout << "Ошибка выделения памяти" << endl;
            }

            return block;
        }

        void free(void* block) {
            // Поиск блока памяти по адресу
            // Бежим по элементам вектора (auto - ключевое слово, используемое для определения типа итерируемых элементов)
            for (auto& memoryBlock : memoryBlocks) {
                if (static_cast<char*>(memory) + memoryBlock.address == block) {
                    memoryBlock.used = false; // Пометить блок памяти как неиспользуемый
                    break;
                }
            }
            int count = 0;
            while (true)
            {
                for (int i = 1; i < memoryBlocks.size(); i++)
                {
                    if (!memoryBlocks[i - 1].used && !memoryBlocks[i].used)
                    {
                        memoryBlocks[i - 1].size = memoryBlocks[i - 1].size + memoryBlocks[i].size;
                        memoryBlocks.erase(memoryBlocks.cbegin() + i);
                    }
                }
                count++;
                if (count == 4) break;
            }
        }

        void printMemoryUsage() {
            cout << "Размер памяти, зарезервированный транслятором: " << reservedSize << endl;
            // Бежим по элементам вектора (auto - ключевое слово, используемое для определения типа итерируемых элементов)
            for (const auto& memoryBlock : memoryBlocks) {
                cout << "Размер блока: " << memoryBlock.size << ", Статус: " << (memoryBlock.used ? "используется" : "не используется")
                    << ", Адрес верхнеуровневого сегмента: " << memoryBlock.address << endl;
            }
        }

    private:
        // Выделение памяти размером size
        void* allocateMemory(size_t size) {
            // функция operator new предназначена для выделения памяти размером size в динамической памяти.
            return operator new(size);
        }

        // Освобождение зарезервированной памяти
        void freeMemory(void* block) {
            operator delete(block);
        }
    };

    //int main() {

        //setlocale(LC_ALL, "Russian");

        //Heap heap(1024); // Выделение памяти размером 1024

        //void* block1 = heap.allocate(100); // Выделение блока памяти размером 100
        //void* block2 = heap.allocate(200); // Выделение блока памяти размером 200
        //void* block3 = heap.allocate(300); // Выделение блока памяти размером 300
        //void* block4 = heap.allocate(400); // Выделение блока памяти размером 400
        //void* block5 = heap.allocate(500); // Выделение блока памяти размером 500

        //heap.printMemoryUsage(); // Вывод информации о памяти

        //heap.free(block2); // Освобождение памяти для блока 2
        //heap.free(block4); // Освобождение памяти для блока 4
        //heap.free(block3); // Освобождение памяти для блока 3

        //heap.printMemoryUsage(); // Вывод информации о памяти
        //void* block6 = heap.allocate(1200); // Выделение блока памяти размером 600 (запрос памяти чуть большего размера, чем можно выделить для одного сегмента)

        //void* block7 = heap.allocate(600); // Выделение блока памяти размером 300
        //void* block8 = heap.allocate(600); // Выделение блока памяти размером 300
        //void* block9 = heap.allocate(600); // Выделение блока памяти размером 300

        //heap.printMemoryUsage(); // Вывод информации о памяти

        //return 0;
    //}

}