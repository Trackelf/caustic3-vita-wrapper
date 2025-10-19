#pragma once
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
    void *text; size_t text_size;
    void *data; size_t data_size;
    void *bss;  size_t bss_size;
    size_t size; void *reloc_base;
    int num_symbols;
    void *symbol_table;
    void *string_table;
} so_ctx_t;

int so_init(so_ctx_t *ctx, const void *elf_data, size_t size);
int so_relocate(so_ctx_t *ctx);
int so_resolve(so_ctx_t *ctx, void *(*resolve)(const char *symbol));
int so_flush_caches(so_ctx_t *ctx);
int so_initialize(so_ctx_t *ctx);
void* so_find_symbol(so_ctx_t *ctx, const char *name);
#ifdef __cplusplus
}
#endif
