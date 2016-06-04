/*
 * Implementation of memory pool with a singly linked list, with O(1) alloc and O(1) free
 * Doen't work properly in case of double free
 * Size of pool is a template argumemt
 */

#include<iostream>
#include<cstdlib>
#include<tuple>
#include<stdio.h>

using namespace std;

// naming definitions I followed:
// chunkSize = size of a single object + size of struct Addr
// blockSize = size of entire memory pool = chunkSize * MAX_LIVE_OBJECTS
// maxAddress = the maximum address in a memory pool
// freeMemoryBlock = head of the linked list of free Chunks

struct Addr {
    struct Addr* next;
};

template<class T, int MAX_LIVE_OBJECTS>
class MemoryPool {
    private:
        char memoryBlock[MAX_LIVE_OBJECTS * (sizeof(struct Addr) + sizeof(T))];
        int chunkSize = sizeof(struct Addr) + sizeof(T);
        int blockSize = MAX_LIVE_OBJECTS * chunkSize;
        unsigned long maxAddress = (unsigned long) memoryBlock + blockSize;
        struct Addr* freeMemoryBlock;
    public:
        MemoryPool();
        void printFreeBlocks();
        std::tuple<T*, bool> alloc();
        void my_free(T * ptr);
};

template<class T, int MAX_LIVE_OBJECTS>
MemoryPool<T,MAX_LIVE_OBJECTS>::MemoryPool() : freeMemoryBlock(NULL) {
    for (int i = 0; i < MAX_LIVE_OBJECTS; i++) {
        struct Addr * current = (struct Addr *) ((char *) memoryBlock + i * (sizeof(struct Addr) + sizeof(T)));
        current->next = freeMemoryBlock;
        freeMemoryBlock = current;
    }
}

template<class T, int MAX_LIVE_OBJECTS>
std::tuple<T*, bool> MemoryPool<T, MAX_LIVE_OBJECTS>::alloc() {
    if (freeMemoryBlock == NULL) {
        std::tuple<T*, bool> ret (NULL, false);
        return ret;
    } else {
        struct Addr * temp = freeMemoryBlock;
        T* allocatedObject = (T*)((char*) freeMemoryBlock + sizeof(struct Addr));
        freeMemoryBlock = freeMemoryBlock->next;
        // set the next of currently allocated chunk to NULL
        // will be used to partially check the problem of double freeing
        temp->next = NULL;
        std::tuple<T*, bool> ret (allocatedObject, true);
        return ret;
    }
}

// free in this case will work with O(1) only when we can ensure that there cannot be
// the problem of double freeing. If we double free an object then the entire free list
// can be corrupted. Though I have handled some cases of double freeing in O(1), but all
// of them cannot be handelled. To overcome this scenario we need to parse the free list
// which will give us a complexity of O(n) where n is its size. Other thing we can do is
// we can make the Addr struct doubly linked list. In this case the complexity will still
// be O(1). Refer first-dll.cpp. Also refer first-sll-hack.cpp, in which I abused some
// conventions to make this work with singly linked list also
template<class T, int MAX_LIVE_OBJECTS>
void MemoryPool<T, MAX_LIVE_OBJECTS>::my_free(T * ptr) {
    struct Addr* current = (struct Addr *) ptr - 1;
    // check if its a valid address
    if ((unsigned long) ptr < maxAddress &&
            (unsigned long) ptr > (unsigned long) memoryBlock &&
            (((unsigned long) current - (unsigned long) memoryBlock) % chunkSize == 0) ) {
        // check if the address is not being double freed
        if (current->next == NULL && freeMemoryBlock != current) {
            current->next = freeMemoryBlock;
            freeMemoryBlock = current;
        }
    } else{
        cerr << "Invalid Address for free\n";
    }
}

class testclass {
    int i;
};

template<class T, int MAX_LIVE_OBJECTS>
void MemoryPool<T, MAX_LIVE_OBJECTS>::printFreeBlocks() {
    struct Addr * temp = freeMemoryBlock;
    printf("free Addresses: [ ");
    while (temp != NULL) {
        printf("%p ", temp);
        temp = temp->next;
    }
    printf("]\n");
}

int main() {
    MemoryPool<testclass, 5> mem;

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
        //   cout << "Give command \"y\" to allocate a chunk and \"z <hex addr>\" to free (T*) addr\n\n";
        //}
    }
}
