/*
 * Implementation of memory pool in c with a doubly list list, with O(1) alloc and O(1) free
 * Works properly in case of double free
 * Size of pool is a runtime argumemt
 */

#include<stdio.h>
#include<stdlib.h>

struct Addr {
    struct Addr* next;
    struct Addr* prev;
};

struct MemoryPool {
    void * memoryBlock;
    int chunkSize;
    int blockSize;
    unsigned long maxAddress;
    unsigned int MAX_LIVE_OBJECTS;
    struct Addr* freeMemoryBlock;
};

struct MemoryPool * pool;

void init(unsigned int unitSize, unsigned int maxiumObjects){
    pool = (struct MemoryPool *) malloc (sizeof(struct MemoryPool));
    if (pool != NULL) {
        pool->MAX_LIVE_OBJECTS = maxiumObjects;
        pool->freeMemoryBlock = NULL;
        pool->chunkSize = unitSize + sizeof(struct Addr);
        pool->blockSize = (pool->MAX_LIVE_OBJECTS) * (pool->chunkSize);
        pool->memoryBlock = (void *) malloc(pool->blockSize);
        pool->maxAddress = (unsigned long) pool->memoryBlock + pool->blockSize;
        if (pool->memoryBlock != NULL){
            unsigned int i = 0;
            for (i = 0; i < pool->MAX_LIVE_OBJECTS; i++){
                struct Addr * current = (struct Addr *) ((char *) pool->memoryBlock + i * (sizeof(struct Addr) + unitSize));
                current->prev = NULL;
                current->next = pool->freeMemoryBlock;
                if (pool->freeMemoryBlock != NULL){
                    pool->freeMemoryBlock->prev = current;
                }
                pool->freeMemoryBlock = current;
            }
        }
        else {
            printf("Error allocating %d bytes\n", pool->blockSize);
            exit(1);
        }
    } else {
        printf("Error allocating pool structure\n");
    }
}

// return pointer is of type void *. Cast it to whatever you intend.
// Its the programmer's responsibility that size of target type should be <= unitSize
// Note that this is highly vulnerable of buffer overflow attacks.
// Consider a scenario where we cast the return pointer to a struct that
// contains a char buffer. If we dont fill the buffer carefully, our
// entire free list can be corrupted, and hence the program can crash
void* alloc() {
    if (pool->freeMemoryBlock == NULL) {
        return NULL;
    } else {
        struct Addr * temp = pool->freeMemoryBlock;
        void* allocatedObject = (void *)((char*) pool->freeMemoryBlock + sizeof(struct Addr));
        pool->freeMemoryBlock = pool->freeMemoryBlock->next;
        // maintain dll property
        if (pool->freeMemoryBlock != NULL) {
            pool->freeMemoryBlock->prev = NULL;
        }
        // set the next and prev of currently allocated chunk to NULL
        // will be used to check the problem of double freeing
        temp->next = NULL;
        temp->prev = NULL;
        return allocatedObject;
    }
}

void free(void * ptr) {
    struct Addr* current = (struct Addr *) ptr - 1;
    // check if its a valid address
    if ((unsigned long) ptr < pool->maxAddress &&
            (unsigned long) ptr > (unsigned long) pool->memoryBlock &&
            (((unsigned long) current - (unsigned long) pool->memoryBlock) % pool->chunkSize == 0) ) {
        //check if address is being double freed
        if (current->next == NULL && current->prev == NULL && pool->freeMemoryBlock != current) {
            current->next = pool->freeMemoryBlock;
            current->prev = NULL;
            // maintain dll property
            if (pool->freeMemoryBlock != NULL){
                pool->freeMemoryBlock->prev = current;
            }
            pool->freeMemoryBlock = current;
        }
    } else {
        printf("Invalid address for free\n");
    }
}

void printFreeBlocks() {
    struct Addr * temp = pool->freeMemoryBlock;
    printf("free Addresses: [ ");
    while (temp != NULL) {
        printf("%p ", temp);
        temp = temp->next;
    }
    printf("]\n");
}

struct test {
    int a;
};

int main() {
    init(sizeof(struct test), 5);

    char c;
    void *x;

    printf("Give command \"y\" to allocate a chunk and \"z <hex addr>\" to free (T*) addr\n\n");

    printFreeBlocks();

    while (1) {
        scanf("%c", &c);
        if (c == 'y') {
            x = alloc();
            if (x != NULL) {
                printf("Allocated address: %p\n\n", x);
                printFreeBlocks();
            } else {
                printf("Allocation failed: No free space\n");
            }
        } else if(c == 'z') {
            unsigned long inp;
            scanf("%lx", &inp);
            free((void *) inp);
            printFreeBlocks();
        }
    }
}

