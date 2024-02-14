#include <iostream>
#include <vector>

using namespace std;

namespace allocatorh {

    // ��������� ��� �������� ���������� � ������ ����� ������
    struct MemoryBlock {
        size_t size;  // ������ �����
        bool used;    // ������ ����� (������������/�� ������������)
        size_t address; // ����� ���������������� ��������
        // ����� ���������������� �������� � ���� ��������� - ��� �������� (� ������) ������������ ������ ������, 
        // ����������������� ������������, �� �������� ����� ������ � ������� memoryBlocks.
    };

    // ������������ ���������� �������
    extern class Heap {
    private:
        void* memory; // ��������� �� ������, ����������������� ������������ (������������ ������ ������ �����)
        size_t reservedSize; // ������ ������, ����������������� ������������
        vector<MemoryBlock> memoryBlocks; // ������ ��� �������� ���������� � ������ ����� ������

    public:
        Heap(size_t size) {
            memory = allocateMemory(size); // ��������������� ������ �������� size (������������ ������ ������ �����)
            reservedSize = size;
        }

        ~Heap() {
            freeMemory(memory); // ���������� ����������������� ������
        }

        void* allocate(size_t size) {
            void* block = nullptr; // ��������� �� ������ ����� ������

            // ����� ���������� ����� ������
            // ����� �� ��������� ������� (auto - �������� �����, ������������ ��� ����������� ���� ����������� ���������)
            for (auto& memoryBlock : memoryBlocks) {
                // ���� ���� �� ������������ � ��� ������ >= ������ 
                if (!memoryBlock.used && memoryBlock.size >= size) {
                    memoryBlock.used = true; // �������� ���� ������ ��� ������������
                    // � C++ ���� ����������� ��� �������������� �������� � ����������� �� ��� void*. ����� ��������� �� ���������� � ��������������� ����������
                    // ������� ��������������� � char*
                    // ������ ������ + �������� ��������� �����
                    block = static_cast<char*>(memory) + memoryBlock.address; // ������� ��������� �� ������ ����� ������
                    break;
                }
            }

            // ���� �� ������� ����� ��������� ����, ������� ����� ���� ������
            // ���� �� ������� ����� ��������� �������� � ������ � ������ ������ ��� ������������ ������ ��� ������ �����
            if (block == nullptr && size <= reservedSize) {
                MemoryBlock newBlock = { size, true, memoryBlocks.empty() ? 0 : (memoryBlocks.back().address + memoryBlocks.back().size) }; // ������� ����� ���� ������
                block = static_cast<char*>(memory) + newBlock.address; // ������� ��������� �� ������ ������ ����� ������
                memoryBlocks.push_back(newBlock); // �������� ���������� � ����� ����� � ������ memoryBlocks
            }
            if (size > reservedSize)
            {
                cout << "������ ��������� ������" << endl;
            }

            return block;
        }

        void free(void* block) {
            // ����� ����� ������ �� ������
            // ����� �� ��������� ������� (auto - �������� �����, ������������ ��� ����������� ���� ����������� ���������)
            for (auto& memoryBlock : memoryBlocks) {
                if (static_cast<char*>(memory) + memoryBlock.address == block) {
                    memoryBlock.used = false; // �������� ���� ������ ��� ��������������
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
            cout << "������ ������, ����������������� ������������: " << reservedSize << endl;
            // ����� �� ��������� ������� (auto - �������� �����, ������������ ��� ����������� ���� ����������� ���������)
            for (const auto& memoryBlock : memoryBlocks) {
                cout << "������ �����: " << memoryBlock.size << ", ������: " << (memoryBlock.used ? "������������" : "�� ������������")
                    << ", ����� ���������������� ��������: " << memoryBlock.address << endl;
            }
        }

    private:
        // ��������� ������ �������� size
        void* allocateMemory(size_t size) {
            // ������� operator new ������������� ��� ��������� ������ �������� size � ������������ ������.
            return operator new(size);
        }

        // ������������ ����������������� ������
        void freeMemory(void* block) {
            operator delete(block);
        }
    };

    //int main() {

        //setlocale(LC_ALL, "Russian");

        //Heap heap(1024); // ��������� ������ �������� 1024

        //void* block1 = heap.allocate(100); // ��������� ����� ������ �������� 100
        //void* block2 = heap.allocate(200); // ��������� ����� ������ �������� 200
        //void* block3 = heap.allocate(300); // ��������� ����� ������ �������� 300
        //void* block4 = heap.allocate(400); // ��������� ����� ������ �������� 400
        //void* block5 = heap.allocate(500); // ��������� ����� ������ �������� 500

        //heap.printMemoryUsage(); // ����� ���������� � ������

        //heap.free(block2); // ������������ ������ ��� ����� 2
        //heap.free(block4); // ������������ ������ ��� ����� 4
        //heap.free(block3); // ������������ ������ ��� ����� 3

        //heap.printMemoryUsage(); // ����� ���������� � ������
        //void* block6 = heap.allocate(1200); // ��������� ����� ������ �������� 600 (������ ������ ���� �������� �������, ��� ����� �������� ��� ������ ��������)

        //void* block7 = heap.allocate(600); // ��������� ����� ������ �������� 300
        //void* block8 = heap.allocate(600); // ��������� ����� ������ �������� 300
        //void* block9 = heap.allocate(600); // ��������� ����� ������ �������� 300

        //heap.printMemoryUsage(); // ����� ���������� � ������

        //return 0;
    //}

}