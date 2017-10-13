#ifndef XSTRING
#define XSTRING

typedef struct xstring {
    char *str;
    struct xstring *next;
} xstring; 


//////////////////////////////////////////////////////////////////////////
void*    allocate(size_t size);

#ifdef USE_STD_MALLOC
#define dellocate(ptr) free(ptr);
#else
#define dellocate(ptr) mem_ret(ptr);
#endif

//////////////////////////////////////////////////////////////////////////
xstring* xstring_new(size_t size);
void     xstring_delete(xstring **head);
int      xstring_size(xstring *head);

//////////////////////////////////////////////////////////////////////////
size_t   count(char* src, char* dst);
char*    replace(char *src, char *old_val, char *new_val);
xstring* split(char *str, char *delimter);
char*    strip(char *str);
char*    lstrip(char *str);
char*    rstrip(char *str);
int      start_with(char *str, char *sym);
int      end_with(char *str, char *sym);
char*    uniq_seq_repeat_chars(char *str);

#endif
