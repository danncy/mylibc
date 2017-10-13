#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mem_pool.h"

static byte*          g_pool_buff[MEM_POOL_MAX]     = {0};
static MemBlockDlist* g_pool_objects[MEM_POOL_MAX]  = {0};
static MemBlockDlist* g_pool_freelist[MEM_POOL_MAX] = {0};
static MemBlockDlist* g_pool_cursor[MEM_POOL_MAX]   = {0};

MemBlockDlist* mem_block_dlist_head(size_t index) {
    if (index < 0 || index >= MEM_POOL_MAX) 
        return NULL;

    return g_pool_objects[index];
}

void mem_pool_cursor_init() {
    for(size_t i=0; i<MEM_POOL_MAX; i++)
        g_pool_cursor[i] = mem_block_dlist_head(i);
}

static MemBlockDlist* mem_block_dlist_alloc(byte* buf, int bk_size, int bk_num){
    MemBlockDlist *head, *curr, *next;
    int i;

    head = (MemBlockDlist *)buf;
    head->size = MEM_BLOCK_FREE;
    head->ptr  = (byte*)head + sizeof(MemBlockDlist);
    head->prev = NULL;
    head->next = NULL;

    curr = head;
    for (i=1; i<bk_num; i++) {
        next = (MemBlockDlist*)((byte*)curr + bk_size);
        next->size = MEM_BLOCK_FREE;
        next->ptr  = (byte*)next + sizeof(MemBlockDlist);

        curr->next = next;
        next->prev = curr;
        next->next = NULL;

        curr = next;
    }
    return head;
}

void mem_pool_dlist_init() {
    size_t l, i, s, n, b;
    for (l=0; l<MEM_BLOCK_TYPE_NUM; l++) {
        i = 1 << l;
        b = i * MEM_BLOCK;
        s = b + sizeof(MemBlockDlist);
        n = mem_block_num(b);

        g_pool_buff[l] = (byte *)malloc(s * n);
        if (NULL == g_pool_buff[l]) {
            break;
        }
        g_pool_objects[l] = mem_block_dlist_alloc(g_pool_buff[l], s, n);
    }

    mem_pool_cursor_init();
}

void mem_pool_dlist_free() {
    for (int i=0; i<MEM_BLOCK_TYPE_NUM; i++) {
        if (NULL != g_pool_buff[i]) {
            free(g_pool_buff[i]);

            g_pool_buff[i]     = NULL;
            g_pool_objects[i]  = NULL;
            g_pool_freelist[i] = NULL;
            g_pool_cursor[i]   = NULL;
        }
    }
}

MemBlockDlist* mem_free_block_get(int index) {
    if (index < 0 || index >= MEM_POOL_MAX) return NULL;

    MemBlockDlist* tmp = g_pool_freelist[index];
    if (!tmp) return NULL;

    g_pool_freelist[index] = tmp->next;
    g_pool_freelist[index]->prev = NULL;
    tmp->next = NULL;

    return tmp;
}

void* mem_realloc(size_t size) {
    int index = mem_pool_index(size);
    if (index < 0 || index >= MEM_POOL_MAX) 
        return NULL;
    
    MemBlockDlist* ptr = mem_free_block_get(index);
    if (ptr) {
        ptr->size = size;
        return ptr->ptr;
    }

    MemBlockDlist* cursor = g_pool_cursor[index];
    if (NULL == cursor) {
        printf("pool index=%d memory leak or queue is all used.\n", index);
        return NULL;
    }
    
    if (MEM_BLOCK_FREE == cursor->size) {
        g_pool_cursor[index] = cursor->next;
        cursor->size = size;
        return cursor->ptr;
    }

    return NULL;
}

void  mem_release(void** ptr) {
    if (NULL == ptr || NULL == *ptr) 
        return;

    size_t len = sizeof(MemBlockDlist);
    MemBlockDlist *block = (MemBlockDlist *)((byte*)(*ptr) - len);

    int index = mem_pool_index(block->size);
    if (index < 0 || index >= MEM_POOL_MAX) 
        return;

    memset((byte*)(*ptr), 0, block->size);
    MemBlockDlist* top = g_pool_freelist[index];

    g_pool_freelist[index] = block;
    if (top) {    
        g_pool_freelist[index]->next = top;
    }

    g_pool_freelist[index]->size = MEM_BLOCK_FREE;
}
