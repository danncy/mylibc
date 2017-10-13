nclude <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mem_pool.h"

static byte*     g_pool_mem[MEM_POOL_MAX] = {0};
static MemBlock* g_pool[MEM_POOL_MAX]     = {0};
const float log2 = log(2.0);
int mem_pool_index(size_t size) {
    if (size > 0 && size <= MEM_BLOCK) return 0;

    size_t s = 1;
    size_t t = size;
    size_t i;

    i = log((float)t)/log2; 
    s = s<<i;
    return s == size ? i-3 : i-2;
}

MemBlock* mem_block_head(size_t index) {
    if (index < 0 || index >= MEM_POOL_MAX) 
        return NULL;
    
    return g_pool[index];
}

MemBlock* mem_block_head_get(size_t size) {
    if (size > MEM_BLOCK_BASE/2 || size <= 0) {
        printf("size=%d too big to mem pool. using malloc plz\n", size);
        return NULL;
    }
    return mem_block_head(mem_pool_index(size));
}

int mem_block_num(size_t size) {
    return MEM_BLOCK_BASE/size;// size > 0 is true
}

static MemBlock* mem_block_alloc(byte* buf, int block_size, int block_num) {
    MemBlock *head, *curr, *next;
    int i;

    head = (MemBlock *)buf;
    head->size = MEM_BLOCK_FREE;
    head->ptr  = (byte*)head + sizeof(MemBlock);
    head->next = NULL;

    curr = head;
    for (i=1; i<block_num; i++) {
        next = (MemBlock*)((byte*)curr + block_size);
        next->size = MEM_BLOCK_FREE;
        next->ptr = (byte*)next + sizeof(MemBlock);
        curr->next = next;
        next->next = NULL;
        curr = next;
    }
    return head;
}

void mem_pool_init() {
    size_t l, i, s, n, b;
    for (l=0; l<MEM_BLOCK_TYPE_NUM; l++) {
        i = 1 << l;
        b = i * MEM_BLOCK;
        s = b + sizeof(MemBlock);
        n = mem_block_num(b);

        g_pool_mem[l] = (byte *)malloc(s * n);
        if (NULL == g_pool_mem[l]) {
            break;
        }
        g_pool[l] = mem_block_alloc(g_pool_mem[l], s, n);
    }
    return;
}

void mem_pool_free() {
    for (int i=0; i<MEM_BLOCK_TYPE_NUM; i++) {
        if (NULL != g_pool_mem[i]) {
            free(g_pool_mem[i]);
            g_pool_mem[i] = NULL;
            g_pool[i] = NULL;
        }
    }
    return;
}

void* mem_get(size_t size) {
    MemBlock* head = mem_block_head_get(size);
    if (NULL == head) return NULL;

    do {
        if (head->size == MEM_BLOCK_FREE) {
            head->size = size;
            return head->ptr;
        } else {
            head = head->next;
        }
    } while (head->next);

    return NULL;
}

int mem_free(void **ptr) {
    if (NULL == ptr || NULL == *ptr) return 0;
    
    MemBlock *block = (MemBlock *)((byte*)(*ptr) - sizeof(MemBlock));
    block->size = MEM_BLOCK_FREE;
    
    return 1;
}
