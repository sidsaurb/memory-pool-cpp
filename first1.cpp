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
    public:
        void* memoryBlock;
        int blockSize;
        int chunkSize;
        unsigned long maxAddress;
        struct Addr* freeMemoryBlock;
        MemoryPool();
        ~MemoryPool();
        std::tuple<T*, bool> alloc();
        void my_free(T * ptr);
};

template<class T, int MAX_LIVE_OBJECTS>
MemoryPool<T,MAX_LIVE_OBJECTS>::MemoryPool() : freeMemoryBlock(NULL) {
    chunkSize = sizeof(struct Addr) + sizeof(T);
    blockSize = MAX_LIVE_OBJECTS * chunkSize;
    memoryBlock = (void*) malloc(blockSize);
    maxAddress = (long) memoryBlock + blockSize;
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
    if (freeMemoryBlock == NULL) {
        std::tuple<T*, bool> ret (NULL, false);
        return ret;
    } else {
        T* allocatedObject = (T*)((char*) freeMemoryBlock + sizeof(struct Addr));
        freeMemoryBlock = freeMemoryBlock->next;
        std::tuple<T*, bool> ret (allocatedObject, true);
        return ret;
    }
}

template<class T, int MAX_LIVE_OBJECTS>
void MemoryPool<T, MAX_LIVE_OBJECTS>::my_free(T * ptr) {
    struct Addr* address = (struct Addr *) ptr - 1;
    if ((unsigned long) ptr < maxAddress &&
            (unsigned long) ptr > (unsigned long) memoryBlock &&
            (((unsigned long) address - (unsigned long) memoryBlock) % chunkSize == 0) ) {
        if (address != freeMemoryBlock) {
            address->next = freeMemoryBlock;
            freeMemoryBlock = address;
            //cout << "found\n";
        }
    }
}

class testclass {
    int i;
};

void printFreeBlocks(MemoryPool<testclass, 5> *pool) {

    int size = sizeof(struct Addr) + sizeof(testclass);
    struct Addr * temp = pool->freeMemoryBlock;
    printf("[ ");
    while (temp != NULL) {
        printf("%p ", temp);
        temp = temp->next;
    }
    printf("]\n");
    //printf("%p\n", mem.memoryBlock);
    //printf("%lx\n", mem.maxAddress);

}

int main() {
    MemoryPool<testclass, 5> mem;

    char c;
    testclass * addr;
    bool success;
    std::tuple<testclass*, bool> x;

    printFreeBlocks(&mem);

    while(true){
        scanf("%c", &c);
        if (c == 'a') {
            x = mem.alloc();
            addr = std::get<0>(x);
            success = std::get<1>(x);
            cout << addr << " " << success << "\n";
            printFreeBlocks(&mem);
        } else if(c == 'f') {
            unsigned long x;
            cin >> hex >> x;
            mem.my_free((testclass *) x);
            printFreeBlocks(&mem);

        }

    }
}
