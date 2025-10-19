#include <psp2/io/fcntl.h>
#include <psp2/io/stat.h>
#include <psp2/types.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <psp2/kernel/sysmem.h>

#include "so_util.h"
#include "resolve.h"
#include "libloader.h"
#include "log.h"

static char g_root[256] = "ux0:/data/CAUSTIC3/";
static char g_buf[768];

// local logger (avoid name clash with libm::logf)
static void logmsg(const char *fmt, ...) {
    char line[512];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(line, sizeof(line), fmt, ap);
    va_end(ap);
    debug_log(line);
}

static int starts_with(const char *s, const char *pfx) {
    size_t a = strlen(pfx);
    return strncmp(s, pfx, a) == 0;
}

const char* MapPath_Vita(const char *in) {
    if (!in || !*in) return in;

    if (starts_with(in, "file:///android_asset/")) {
        snprintf(g_buf, sizeof(g_buf), "app0:/assets/%s", in + strlen("file:///android_asset/"));
        return g_buf;
    }
    if (starts_with(in, "assets:/")) {
        snprintf(g_buf, sizeof(g_buf), "app0:/assets/%s", in + strlen("assets:/"));
        return g_buf;
    }
    if (starts_with(in, "/sdcard/")) {
        snprintf(g_buf, sizeof(g_buf), "%s%s", g_root, in + strlen("/sdcard/"));
        return g_buf;
    }
    if (starts_with(in, "/storage/emulated/0/")) {
        snprintf(g_buf, sizeof(g_buf), "%s%s", g_root, in + strlen("/storage/emulated/0/"));
        return g_buf;
    }
    if (starts_with(in, "ux0:/") || starts_with(in, "app0:/")) {
        return in;
    }

    return in;
}

void CausticNative_SetRootPath(const char *path) {
    if (path && *path) {
        snprintf(g_root, sizeof(g_root), "%s", path);
    }

    sceIoMkdir("ux0:/data", 0777);
    sceIoMkdir(g_root, 0777);
    debug_log("SetRootPath aufgerufen.");
}

void CausticRenderer_nativeInitGraphics(int width, int height) {
    debug_log("InitGraphics Dummy ausgeführt.");
}

static void* read_whole_file(const char *path, size_t *out_size) {
    int fd = sceIoOpen(path, SCE_O_RDONLY, 0);
    if (fd < 0) { char m[192]; snprintf(m,sizeof(m),"read_whole_file: open failed %s -> 0x%08X", path, fd); debug_log(m); return NULL; }

    SceOff sz = sceIoLseek(fd, 0, SCE_SEEK_END);
    if (sz <= 0) { sceIoClose(fd); debug_log("read_whole_file: invalid size"); return NULL; }
    sceIoLseek(fd, 0, SCE_SEEK_SET);

    // zuerst Memblock versuchen, ausgerichtet auf 4K
    SceSize need = (SceSize)((sz + 0xFFF) & ~0xFFF);
    SceUID blk = sceKernelAllocMemBlock("soimg", SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE, need, NULL);
    if (blk < 0) blk = sceKernelAllocMemBlock("soimg", SCE_KERNEL_MEMBLOCK_TYPE_USER_RW, need, NULL);

    void *base = NULL;
    if (blk >= 0) {
        if (sceKernelGetMemBlockBase(blk, &base) < 0 || !base) { base = NULL; }
    }

    // Fallback auf malloc mit vergrößertem newlib Heap
    if (!base) {
        base = malloc((size_t)sz);
        if (!base) { sceIoClose(fd); debug_log("read_whole_file: malloc failed"); return NULL; }
    }

    size_t total = 0;
    char *dst = (char*)base;
    const size_t CHUNK = 128 * 1024;
    while (total < (size_t)sz) {
        size_t want = ((size_t)sz - total) > CHUNK ? CHUNK : ((size_t)sz - total);
        int rd = sceIoRead(fd, dst + total, want);
        if (rd <= 0) { debug_log("read_whole_file: short read"); sceIoClose(fd); return NULL; }
        total += (size_t)rd;
    }
    sceIoClose(fd);

    if (out_size) *out_size = (size_t)sz;
    char m[128]; snprintf(m, sizeof(m), "read_whole_file: ok, %d bytes", (int)sz); debug_log(m);
    return base; // absichtlich nicht freigeben
}



int Loader_EarlyInit(void) {
    debug_log("Loader (so_util) begin");
    const char *lib_path = "app0:/lib/libcaustic.so";
    size_t so_size = 0;
    void *so_image = read_whole_file(lib_path, &so_size);
    if (!so_image) { debug_log("Loader: read failed"); return -1; }

    so_ctx_t ctx;
    int r = so_init(&ctx, so_image, so_size);
    char msg[128]; snprintf(msg, sizeof(msg), "so_init -> %d", r); debug_log(msg);
    if (r < 0) return r;

    so_relocate(&ctx);
    so_resolve(&ctx, resolve_symbol);
    so_flush_caches(&ctx);
    so_initialize(&ctx);

    debug_log("Caustic .so loaded via so_util.");
    return 0;
}