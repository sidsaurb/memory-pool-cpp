/*
 * Implementation of memory pool with a doubly linked list, with O(1) alloc and O(1) free
 * Works properly in case of double free
 * Size of pool is a runtime argumemt
 */

#include<iostream>
#include<cstdlib>
#include<tuple>
#include<stdio.h>

using namespace std;

struct Addr {
    struct Addr* next;
    struct Addr* prev;
};

template<class T>
class MemoryPool {
    private:
        void * memoryBlock;
        int chunkSize;
        int blockSize;
        unsigned long maxAddress;
        unsigned int MAX_LIVE_OBJECTS;
        struct Addr* freeMemoryBlock;
    public:
        MemoryPool(unsigned int MAX_LIVE_OBJECTS);
        ~MemoryPool();
        void printFreeBlocks();
        std::tuple<T*, bool> alloc();
        void my_free(T * ptr);
};

template<class T>
MemoryPool<T>::MemoryPool(unsigned int MAX_LIVE_OBJECTS) :
    freeMemoryBlock(NULL), MAX_LIVE_OBJECTS(MAX_LIVE_OBJECTS), memoryBlock(NULL),
    chunkSize(sizeof(struct Addr) + sizeof(T))
{
    blockSize = chunkSize * MAX_LIVE_OBJECTS;
    memoryBlock = (void *) malloc(blockSize);
    maxAddress = (unsigned long) memoryBlock + blockSize;
    if (memoryBlock != NULL) {
        for (unsigned int i = 0; i < MAX_LIVE_OBJECTS; i++) {
            struct Addr * current = (struct Addr *) ((char *) memoryBlock + i * (sizeof(struct Addr) + sizeof(T)));
            current->prev = NULL;
            current->next = freeMemoryBlock;
            if (freeMemoryBlock != NULL){
                freeMemoryBlock->prev = current;
            }
            freeMemoryBlock = current;
        }
    } else {
        cout << "Error allocating " << blockSize << " bytes\n";
        exit(1);
    }
}

template<class T>
MemoryPool<T>::~MemoryPool(){
    free(memoryBlock);
}

template<class T>
std::tuple<T*, bool> MemoryPool<T>::alloc() {
    if (freeMemoryBlock == NULL) {
        std::tuple<T*, bool> ret (NULL, false);
        return ret;
    } else {
        struct Addr * temp = freeMemoryBlock;
        T* allocatedObject = (T*)((char*) freeMemoryBlock + sizeof(struct Addr));
        freeMemoryBlock = freeMemoryBlock->next;
        // maintain dll property
        if (freeMemoryBlock != NULL) {
            freeMemoryBlock->prev = NULL;
        }
        // set the next and prev of currently allocated chunk to NULL
        // will be used to check the problem of double freeing
        temp->next = NULL;
        temp->prev = NULL;
        std::tuple<T*, bool> ret (allocatedObject, true);
        return ret;
    }
}

// this free implementation takes care of double freeing
template<class T>
void MemoryPool<T>::my_free(T * ptr) {
    struct Addr* current = (struct Addr *) ptr - 1;
    // check if its a valid address
    if ((unsigned long) ptr < maxAddress &&
            (unsigned long) ptr > (unsigned long) memoryBlock &&
            (((unsigned long) current - (unsigned long) memoryBlock) % chunkSize == 0) ) {
        // check if address is being double freed
        if (current->next == NULL && current->prev == NULL && freeMemoryBlock != current) {
            current->next = freeMemoryBlock;
            current->prev = NULL;
            // maintain dll property
            if (freeMemoryBlock != NULL){
                freeMemoryBlock->prev = current;
            }
            freeMemoryBlock = current;
        }
    } else {
        cerr << "Invalid address for free\n";
    }
}

template<class T>
void MemoryPool<T>::printFreeBlocks() {
    struct Addr * temp = freeMemoryBlock;
    printf("free Addresses: [ ");
    while (temp != NULL) {
        printf("%p ", temp);
        temp = temp->next;
    }
    printf("]\n");
}

class testclass {
    int i;
};

int main() {
    MemoryPool<testclass> mem(5);

    char c;
    testclass * addr;
    bool success;
    std::tuple<testclass*, bool> x;

    cout << "Give command \"y\" to allocate a chunk and \"z <hex addr>\" to free (T*) addr\n\n";

    mem.printFreeBlocks();

    while (true) {
        scanf("%c", &c);
        if (c == 'y') {
            x = mem.alloc();
            addr = std::get<0>(x);
            success = std::get<1>(x);
            if (success) {
                cout << "Allocated address: " << addr << "\n\n";
                mem.printFreeBlocks();
            } else {
                cout << "Allocation failed: No free space\n";
            }
        } else if(c == 'z') {
            unsigned long x;
            cin >> hex >> x;
            mem.my_free((testclass *) x);
            mem.printFreeBlocks();
        }
        //else {
        //    cout << "Give command \"y\" to allocate a chunk and \"z <hex addr>\" to free (T*) addr\n\n";
        //}
    }
}
