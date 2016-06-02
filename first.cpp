#include<iostream>
#include<cstdlib>
#include<tuple>
#include<stdio.h>

using namespace std;

template<class T, int MAX_LIVE_OBJECTS>
class MemoryPool {
    private:
//        struct
//        int blockSize = MAX_LIVE_OBJECTS * (sizeof(int) + sizeof(T));
    public:
        char* memoryBlock;
        int* freeMemoryBlock;
        MemoryPool();
        ~MemoryPool();
//        std::tuple<int, bool> alloc();
//        void free();
};

template<class T, int MAX_LIVE_OBJECTS>
MemoryPool<T,MAX_LIVE_OBJECTS>::MemoryPool() : freeMemoryBlock(NULL) {
    memoryBlock = (char*) malloc(MAX_LIVE_OBJECTS * (sizeof(int) + sizeof(T)));
    if (memoryBlock != NULL) {
        for (int i = 0; i < MAX_LIVE_OBJECTS; i++) {
            int * current = (int *) (memoryBlock + i * (sizeof(int) + sizeof(T)));
            *current = freeMemoryBlock;
            freeMemoryBlock = current;
        }
    }
}

template<class T, int MAX_LIVE_OBJECTS>
MemoryPool<T, MAX_LIVE_OBJECTS>::~MemoryPool(){
    free(memoryBlock);
}

class testclass {
    int i;
};

int main() {
    MemoryPool<testclass, 5> mem;
    int size = sizeof(int) + sizeof(testclass);
    int * temp = mem.freeMemoryBlock;
    for (int i = 0; i < 5; i++) {
        printf("%p\n", temp);
        temp = *temp;
    }
    printf("%p\n", mem.memoryBlock);
    mem.freeMemoryBlock;
}
