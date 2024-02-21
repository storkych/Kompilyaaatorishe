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
    class Heap {
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
            int i = 0;
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

                    // Если размер блока больше чем нужно, добавляем пустой блок справа
                    if (memoryBlock.size > size) {
                        memoryBlocks.insert(memoryBlocks.begin() + i + 1, { memoryBlock.size - size, false, memoryBlock.address + size });
                    }

                    // Обновляем размер текущего блока
                    memoryBlock.size = size;
                    break;
                }
                i++;
            }

            // Если не удалось найти свободный блок, создаем новый блок памяти
            // Если не удалось найти свободных участков в памяти и размер меньше чем максимальный размер для одного блока
            if (block == nullptr && size <= reservedSize) {
                MemoryBlock newBlock = { size, true, memoryBlocks.empty() ? 0 : (memoryBlocks.back().address + memoryBlocks.back().size) }; // Создать новый блок памяти
                block = static_cast<char*>(memory) + newBlock.address; // Вернуть указатель на начало нового блока памяти
                memoryBlocks.push_back(newBlock); // Добавить информацию о новом блоке в вектор memoryBlocks
            }
            try {
                if (size > reservedSize) {
                    throw "Ошибка выделения памяти";
                }
            }
            catch (const char* errorMessage) {
                cout << errorMessage << endl;
            }

            return block;
        }


        void free(void* block) {
            // Поиск блока памяти по адресу
            // Бежим по элементам вектора (auto - ключевое слово, используемое для определения типа итерируемых элементов)
            int i = 0; // удаляемый блок 
            for (auto& memoryBlock : memoryBlocks) {
                if (static_cast<char*>(memory) + memoryBlock.address == block) {
                    memoryBlock.used = false; // Пометить блок памяти как неиспользуемый
                    cout << "Удаляем элемент: " << i << "  Размер: " << memoryBlocks[i].size << endl;
                    break;
                }
                i++;
            }
            // Если элемент не первый, тогда нужна проверка для удаляемого и предыдущего
            if (i != 0 && !memoryBlocks[i - 1].used) {
                cout << "Удаляем предыдущий" << endl;
                // объединяем предыдущий и удаляемый 
                memoryBlocks[i - 1].size = memoryBlocks[i - 1].size + memoryBlocks[i].size;
                memoryBlocks.erase(memoryBlocks.cbegin() + i);
                // i - стал следущим элементом, поэтому возвращаем его на нужный
                i -= 1;
            }
            // Если он не последний (есть следущющий) и следующий свободен
            if (i != memoryBlocks.size() - 1 && !memoryBlocks[i + 1].used) {
                cout << "Удаляем следующий" << endl;
                // объединяем нынешний и следущий 
                memoryBlocks[i].size = memoryBlocks[i].size + memoryBlocks[i + 1].size;
                memoryBlocks.erase(memoryBlocks.cbegin() + i + 1);
            }
            // Если последний и неиспользуемый
            if (i == memoryBlocks.size() - 1 && !memoryBlocks[i].used) {
                memoryBlocks.erase(memoryBlocks.cbegin() + i);
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
};