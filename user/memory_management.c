#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"
#include "user/memory_management.h"

#define TRUE 1
#define FALSE 0
#define NULL 0
#define PAGE 4096
#define BLOCK_SIZE sizeof(struct blockHeader)

typedef long Align;

Align forceAlign;

struct blockHeader *freeList = NULL; // start of free-list
struct blockHeader base; // first block


// creates new memory space if there aren't enough blocks of right size
struct blockHeader* moremem(nbytes) {
    void *newAddress;
    struct blockHeader *newBlock;

    if(nbytes < PAGE) {
        nbytes = PAGE;
    }

    newAddress = sbrk(nbytes * BLOCK_SIZE);

    if(newAddress == (void *) -1) {  // if sbrk fails
        return NULL;
    }

    newBlock = (struct blockHeader *) newAddress; 
    newBlock->size = nbytes;
    newBlock->available = TRUE;
    newBlock->nextBlock = NULL;
    return newBlock;
}

// _malloc() allocated nbytes bytes 
void *_malloc(uint nbytes) {

    struct blockHeader *currentBlock; // current and previous blocks
    
    // case 1: no blocks yet in freeList, need to initialise
    if(freeList == NULL) {
        base.size = 0;
        base.nextBlock = NULL;
        base.available = TRUE;
        freeList = &base;
    }

    currentBlock = freeList->nextBlock;

    // case 2: look for free blocks
    while(currentBlock != NULL && currentBlock->size >= nbytes) {
        if(currentBlock->size == nbytes && currentBlock->available == TRUE) {
            currentBlock->available = FALSE;
            currentBlock = currentBlock->nextBlock;
        }
        else if (currentBlock->available == TRUE) {
           if((currentBlock->size - nbytes) > (BLOCK_SIZE + forceAlign)) {
               struct blockHeader new_block; // adds new blocks if currentBlock has enough space for nybtes and a new block
               new_block.size = currentBlock->size - (nbytes + BLOCK_SIZE + forceAlign);
               currentBlock->size = nbytes;
               currentBlock->available = FALSE;
               new_block.available = TRUE;
               currentBlock->nextBlock = &new_block;
               freeList = currentBlock;
           }
        }
    }

    // case 3: no free blocks of right size
    if(currentBlock->size < nbytes) {
        currentBlock = moremem(nbytes); // extra memory created if nbytes < PAGE but it gets taken care of in case 2
    }
}

void _free(void *ptr) {

}