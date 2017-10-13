#ifndef MEM_POOL_H_
#define MEM_POOL_H_

typedef struct MemBlock {
    struct MemBlock* next;
    int size;
    void *ptr;
} MemBlock;

typedef unsigned char byte;

// 8 16 32 64 128 256 512 1024 2048 4096 
// 1 2  4  8  16  32  64  128  256  512
// 0 1  2  3  4   5   6   7    8    9

#define MEM_POOL_MAX 10
#define MEM_BLOCK 8
#define MEM_BLOCK_BASE 8192
#define MEM_BLOCK_TYPE_NUM 10
#define MEM_BLOCK_FREE -1

int       mem_pool_index(size_t size);
void      mem_pool_init();
void      mem_pool_free();
int       mem_block_num(size_t size);
MemBlock* mem_block_head_get(size_t size);
void*     mem_get(size_t size);
int       mem_free(void **ptr);

#define mem_ret(ptr) mem_free((void **)&ptr)

//////////////////////////////////////////////////////////////////////////

typedef struct MemBlockDlist {
    struct MemBlockDlist* prev;
    struct MemBlockDlist* next;
    int size;
    void *ptr;
} MemBlockDlist;

void  mem_pool_dlist_init();
void  mem_pool_dlist_free();
void* mem_realloc(size_t size);
void  mem_release(void** ptr);

#define mem_alloc(s) mem_realloc(s)
#define mem_relex(p) mem_release((void **)&p)

#endif
