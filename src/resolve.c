#include <psp2/io/fcntl.h>
#include <psp2/io/stat.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "log.h"
#include "libloader.h"  // für MapPath_Vita()

// --- Android-like types (minimal) ---
typedef void AAssetManager;
typedef struct AAsset {
    int fd;
    SceOff size;
    SceOff pos;
} AAsset;

// --- __android_log_print ---
static int android_log_print(int prio, const char *tag, const char *fmt, ...) {
    (void)prio; (void)tag;
    char buf[512];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    debug_log(buf);
    return 0;
}

// --- AAssetManager / AAsset shim ---
static AAssetManager* AAssetManager_fromJava(void *env, void *mgr) {
    (void)env; (void)mgr;
    return (AAssetManager*)0x1; // dummy non-null
}

static AAsset* AAssetManager_open(AAssetManager *mgr, const char *name, int mode) {
    (void)mgr; (void)mode;
    // Map auf app0:/assets/<name>
    char path[512];
    snprintf(path, sizeof(path), "app0:/assets/%s", name ? name : "");
    int fd = sceIoOpen(path, SCE_O_RDONLY, 0);
    if (fd < 0) {
        char m[256]; snprintf(m, sizeof(m), "AAsset open fail %s (0x%08X)", path, fd);
        debug_log(m);
        return NULL;
    }
    SceOff end = sceIoLseek(fd, 0, SCE_SEEK_END);
    sceIoLseek(fd, 0, SCE_SEEK_SET);
    AAsset *a = (AAsset*)malloc(sizeof(AAsset));
    if (!a) { sceIoClose(fd); return NULL; }
    a->fd = fd; a->size = end; a->pos = 0;
    char m[256]; snprintf(m, sizeof(m), "AAsset open ok %s (%d bytes)", path, (int)end);
    debug_log(m);
    return a;
}

static int AAsset_read(AAsset *a, void *buf, size_t count) {
    if (!a) return -1;
    int rd = sceIoRead(a->fd, buf, (int)count);
    if (rd > 0) a->pos += rd;
    return rd;
}

static int AAsset_close(AAsset *a) {
    if (!a) return 0;
    sceIoClose(a->fd);
    free(a);
    return 0;
}

static long AAsset_seek(AAsset *a, long offset, int whence) {
    if (!a) return -1;
    SceOff p = sceIoLseek(a->fd, offset, whence);
    if (p >= 0) a->pos = p;
    return (long)p;
}

static off_t AAsset_getLength(AAsset *a) {
    if (!a) return 0;
    return (off_t)a->size;
}

// --- Resolver ---
void *resolve_symbol(const char *s) {
    if (!s) return NULL;

    // logging
    if (!strcmp(s, "__android_log_print")) return (void*)&android_log_print;

    // AAsset / AAssetManager
    if (!strcmp(s, "AAssetManager_fromJava")) return (void*)&AAssetManager_fromJava;
    if (!strcmp(s, "AAssetManager_open"))     return (void*)&AAssetManager_open;
    if (!strcmp(s, "AAsset_read"))            return (void*)&AAsset_read;
    if (!strcmp(s, "AAsset_close"))           return (void*)&AAsset_close;
    if (!strcmp(s, "AAsset_seek"))            return (void*)&AAsset_seek;
    if (!strcmp(s, "AAsset_getLength"))       return (void*)&AAsset_getLength;

    // Unbekanntes Symbol – Loggen
    char m[256]; snprintf(m, sizeof(m), "resolve: unknown symbol '%s'", s);
    debug_log(m);
    return NULL;
}
