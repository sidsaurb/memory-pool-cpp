/*
 * Implementation of memory pool with a singly linked list, with O(1) alloc and O(1) free
 * Works properly in case of double free because of a hack
 * Size of pool is a template argumemt
 */

#include<iostream>
#include<cstdlib>
#include<tuple>
#include<stdio.h>

using namespace std;

#define INVALID_ADDRESS 1

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
        // set the next of currently allocated chunk to INVALID_ADDRESS
        // will be used to check the problem of double freeing
        temp->next = (struct Addr *) INVALID_ADDRESS;
        std::tuple<T*, bool> ret (allocatedObject, true);
        return ret;
    }
}

// free in this case will work with O(1) and also handles the case of double freeing.
// I have defined a special address INVALID_ADDRESS = 1, and assigned it to the next of
// all the allocated address chunks. Thus we only free those chunks which have
// INVALID_ADDRESS as its next pointer
//
// Another hack that is coming to my mind is I can maintain a pointer to the last
// element of freeMemoryBlock in the MemoryPool class itself, and use it to check
// double freeing
template<class T, int MAX_LIVE_OBJECTS>
void MemoryPool<T, MAX_LIVE_OBJECTS>::my_free(T * ptr) {
    struct Addr* current = (struct Addr *) ptr - 1;
    // check if its a valid address
    if ((unsigned long) ptr < maxAddress &&
            (unsigned long) ptr > (unsigned long) memoryBlock &&
            (((unsigned long) current - (unsigned long) memoryBlock) % chunkSize == 0) ) {
        // check if the address is not being double freed
        if (current->next == (struct Addr*) INVALID_ADDRESS) {
            current->next = freeMemoryBlock;
            freeMemoryBlock = current;
        }
    } else {
        cerr << "Invalid address for free\n";
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
        //    cout << "Give command \"y\" to allocate a chunk and \"z <hex addr>\" to free (T*) addr\n\n";
        //}
    }
}
