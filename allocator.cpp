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
    class Heap {
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
            int i = 0;
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

                    // ���� ������ ����� ������ ��� �����, ��������� ������ ���� ������
                    if (memoryBlock.size > size) {
                        memoryBlocks.insert(memoryBlocks.begin() + i + 1, { memoryBlock.size - size, false, memoryBlock.address + size });
                    }

                    // ��������� ������ �������� �����
                    memoryBlock.size = size;
                    break;
                }
                i++;
            }

            // ���� �� ������� ����� ��������� ����, ������� ����� ���� ������
            // ���� �� ������� ����� ��������� �������� � ������ � ������ ������ ��� ������������ ������ ��� ������ �����
            if (block == nullptr && size <= reservedSize) {
                MemoryBlock newBlock = { size, true, memoryBlocks.empty() ? 0 : (memoryBlocks.back().address + memoryBlocks.back().size) }; // ������� ����� ���� ������
                block = static_cast<char*>(memory) + newBlock.address; // ������� ��������� �� ������ ������ ����� ������
                memoryBlocks.push_back(newBlock); // �������� ���������� � ����� ����� � ������ memoryBlocks
            }
            try {
                if (size > reservedSize) {
                    throw "������ ��������� ������";
                }
            }
            catch (const char* errorMessage) {
                cout << errorMessage << endl;
            }

            return block;
        }


        void free(void* block) {
            // ����� ����� ������ �� ������
            // ����� �� ��������� ������� (auto - �������� �����, ������������ ��� ����������� ���� ����������� ���������)
            int i = 0; // ��������� ���� 
            for (auto& memoryBlock : memoryBlocks) {
                if (static_cast<char*>(memory) + memoryBlock.address == block) {
                    memoryBlock.used = false; // �������� ���� ������ ��� ��������������
                    cout << "������� �������: " << i << "  ������: " << memoryBlocks[i].size << endl;
                    break;
                }
                i++;
            }
            // ���� ������� �� ������, ����� ����� �������� ��� ���������� � �����������
            if (i != 0 && !memoryBlocks[i - 1].used) {
                cout << "������� ����������" << endl;
                // ���������� ���������� � ��������� 
                memoryBlocks[i - 1].size = memoryBlocks[i - 1].size + memoryBlocks[i].size;
                memoryBlocks.erase(memoryBlocks.cbegin() + i);
                // i - ���� �������� ���������, ������� ���������� ��� �� ������
                i -= 1;
            }
            // ���� �� �� ��������� (���� ����������) � ��������� ��������
            if (i != memoryBlocks.size() - 1 && !memoryBlocks[i + 1].used) {
                cout << "������� ���������" << endl;
                // ���������� �������� � �������� 
                memoryBlocks[i].size = memoryBlocks[i].size + memoryBlocks[i + 1].size;
                memoryBlocks.erase(memoryBlocks.cbegin() + i + 1);
            }
            // ���� ��������� � ��������������
            if (i == memoryBlocks.size() - 1 && !memoryBlocks[i].used) {
                memoryBlocks.erase(memoryBlocks.cbegin() + i);
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
};