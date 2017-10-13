#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "cstr.h"
#include "mem_pool.h"

void* allocate(size_t size) {
#ifdef USE_STD_MALLOC
    return malloc(size);
#else
    return mem_get(size);
#endif
}

xstring* xstring_new(size_t size) {
    xstring *s = (xstring *)allocate(sizeof(xstring));
    if (!s) return NULL;

    s->str = (char *)allocate(size+1);
    if (!s->str) {
        dellocate(s);
        return NULL;
    }

    s->next = NULL;
    return s;
}

void xstring_delete(xstring** head) {
    xstring *curr = *head;
    xstring *next;

    while(curr) {
        next = curr->next;
        if (curr->str) dellocate(curr->str);
        dellocate(curr);
        curr = next;
    }
    *head = NULL;
    return;
}

int xstring_size(xstring* head) {
    int size = 0;
    while (head) {
        size++;
        head = head->next;
    }
    return size;
}

static void string_copy(char *dst, char *src, int len) {
    if (!dst || !src) return;
    strncpy(dst, src, len);
    dst[len] = '\0';
    return;
}

static void substring_copy(xstring **curr, xstring *next, char *str, int len) {
    string_copy(next->str, str, len);
    (*curr)->next = next;
    (*curr) = (*curr)->next;
}

xstring* split(char* str, char *delimter) {
    char *delimt, *buffer;
    int i = 0;
    int len, match, cnt;
    xstring *head = NULL;
    xstring *next, *curr;
    
    if (NULL == str || NULL == delimter) return NULL;

    delimt = delimter;
    len    = strlen(delimter);
    buffer = str;
    match  = 0;
    cnt    = 0;

    while (*buffer != '\0') {
        if (*buffer == *delimt) {
            delimt++;
            match++; 
        } else if (*buffer != *delimt) {
            delimt = delimter;
        }

        if (match == len) {
            if (NULL == head && cnt > 0) {
                head = xstring_new(cnt);
                if (!head) return NULL;
                string_copy(head->str, str+i-cnt, cnt+1-len);
                curr = head;
            } else if (cnt > 0){
                next = xstring_new(cnt);
                if (!next) return NULL;
                substring_copy(&curr, next, str+i-cnt, cnt+1-len);
            }
            cnt    = 0;
            match  = 0;
            delimt = delimter;
        } else {
            cnt++;
        } 
        i++;
        buffer++;
    }

    if (cnt > 0) {
        next = xstring_new(cnt);
        if (!next) return NULL;
        substring_copy(&curr, next, str+i-cnt, cnt);
    }
    return head;
}

size_t count(char* src, char* dst) {
    size_t cnt = 0;
    char *buf = src;
    char *tmp = dst;
    int sum = 0;
    size_t len = strlen(dst);

    while (*buf != '\0') {
        if (*buf == *tmp) {
            sum++;
            tmp++;
        } else {
            tmp = dst;
            sum = 0;
        }

        if (sum == len) {
            tmp = dst;
            sum = 0;
            cnt++;
        }
        buf++;
    }
    return cnt;
}

char* replace(char *src, char *old_val, char *new_val) {
    if (!src || !old_val || !new_val) return NULL;
    size_t cnt = count(src, old_val);
    if (0 == cnt) return NULL;

    size_t o_len = strlen(old_val);
    size_t n_len = strlen(new_val);
    size_t len   = strlen(src)  + cnt * (n_len - o_len) + 1;

    char *new_str = (char *)allocate(len);
    if (!new_str) return NULL;
    
    char *str = new_str;
    char *buf = src;
    char *tmp = old_val;
    int   sum = 0;

    while (*buf != '\0') {
        if (*buf == *tmp) {
            sum++;
            tmp++;
        } else if (sum > 0){
            tmp = old_val;
            sum = 0;
        }

        if (sum == 0) *new_str++ = *buf;
        if (sum == o_len) {
            tmp = old_val;
            sum = 0;
            for (size_t i=0; i<n_len; i++) *new_str++ = new_val[i];
        }
        buf++;
    }

    for(; sum>0; sum--) *new_str++ = *(buf-sum); 
    *(str+len-1) = '\0';

    return str;
}

char* lstrip(char *str) {
    if (NULL == str) return NULL;

    char *tmp = str;
    int i = 0;

    while (isspace(*tmp++)) i++;
    int len = strlen(str) - i;

    char *s = (char *)allocate(len + 1);
    if (NULL == s) return NULL;

    string_copy(s, str+i, len);
    return s;
}

char* rstrip(char *str) {
    if (NULL == str) return NULL;
    char *tmp = str;
    int len = strlen(str) - 1;

    while (isspace(*(str+len))) len--;

    char *s = (char *)allocate(len + 2);
    if (NULL == s) return NULL;
    
    string_copy(s, str, len+1);
    return s;
}

char* strip(char *str) {
    if (NULL == str) return NULL;

    char *tmp = str;
    int len = strlen(str)-1;
    int i = 0;

    while(isspace(*tmp++)) i++;
    while(isspace(*(str+len))) len--;

    char *s = (char *)allocate(len-i+2);
    if (NULL == s) return NULL;

    string_copy(s, str+i, len-i+1);
    return s;
}

int start_with(char *str, char *sym) {
    if (!str || !sym || 0 == strlen(sym)) return -1;
    return strncmp(str, sym, strlen(sym));
}

int end_with(char *str, char *sym) {
    if (!str || !sym) return -1;
    int l_len = strlen(str);
    int r_len = strlen(sym);

    if (l_len < r_len ) return -1;
    return strncmp(str+l_len-r_len, sym, r_len);
}

int max(int *in_arr, int len) {
    assert(NULL != in_arr);
    int m = in_arr[0];
    for(int i=0; i<len; i++)
        if (m < in_arr[i]) m = in_arr[i];
    return m;
}

char *uniq_seq_repeat_chars(char *str) {
    if (NULL == str) return NULL;

    int rec[256] = {0};
    char *s = (char *)allocate(strlen(str)+1);
    if (NULL == s) return NULL;
    char *s1 = s;
    
    for (int i=0; i<(int)strlen(str); i++) {
        char ch = str[i];
        if (rec[ch] == 0 || 1 != i-rec[ch]) {
            *s = ch;
            s++;
        }
        rec[ch] = i;
    }
    *s = '\0';
    return s1;
}
