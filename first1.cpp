#include<iostream>
#include<cstdlib>
#include<tuple>
#include<stdio.h>

using namespace std;

struct Addr {
    struct Addr* next;
};

template<class T, int MAX_LIVE_OBJECTS>
class MemoryPool {
    private:
//        int blockSize = MAX_LIVE_OBJECTS * (sizeof(int) + sizeof(T));
    public:
        void* memoryBlock;
        struct Addr* freeMemoryBlock;
        MemoryPool();
        ~MemoryPool();
        std::tuple<T*, bool> alloc();
//        void free();
};

template<class T, int MAX_LIVE_OBJECTS>
MemoryPool<T,MAX_LIVE_OBJECTS>::MemoryPool() : freeMemoryBlock(NULL) {
    memoryBlock = (void*) malloc(MAX_LIVE_OBJECTS * (sizeof(struct Addr) + sizeof(T)));
    if (memoryBlock != NULL) {
        for (int i = 0; i < MAX_LIVE_OBJECTS; i++) {
            struct Addr * current = (struct Addr *) ((char *) memoryBlock + i * (sizeof(struct Addr) + sizeof(T)));
            current->next = freeMemoryBlock;
            freeMemoryBlock = current;
        }
    }
}

template<class T, int MAX_LIVE_OBJECTS>
MemoryPool<T, MAX_LIVE_OBJECTS>::~MemoryPool(){
    free(memoryBlock);
}

template<class T, int MAX_LIVE_OBJECTS>
std::tuple<T*, bool> MemoryPool<T, MAX_LIVE_OBJECTS>::alloc() {

}


class testclass {
    int i;
};

int main() {
    MemoryPool<testclass, 5> mem;
    printf("%lu\n", sizeof(struct Addr));
    int size = sizeof(struct Addr) + sizeof(testclass);
    struct Addr * temp = mem.freeMemoryBlock;
    for (int i = 0; i < 5; i++) {
        printf("%p\n", temp);
        temp = temp->next;
    }
    printf("%p\n", mem.memoryBlock);
    mem.freeMemoryBlock;
}
