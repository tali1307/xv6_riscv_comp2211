#include "kernel/types.h"
#include "user/user.h"

void _free(void *ap); // ap - pointer to allocated block to free

void* _malloc(uint nbytes); // nbytes =  number of contiguous bytes required

struct blockHeader {
    int available; // basically my bool
    uint size;
    struct blockHeader *nextBlock;
};