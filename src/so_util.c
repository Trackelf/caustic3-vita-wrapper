#include "so_util.h"
#include <stddef.h>

int so_init(so_ctx_t *ctx, const void *elf_data, size_t size) { return 0; }
int so_relocate(so_ctx_t *ctx) { return 0; }
int so_resolve(so_ctx_t *ctx, void *(*resolve)(const char *symbol)) { return 0; }
int so_flush_caches(so_ctx_t *ctx) { return 0; }
int so_initialize(so_ctx_t *ctx) { return 0; }
void* so_find_symbol(so_ctx_t *ctx, const char *name) { return NULL; }