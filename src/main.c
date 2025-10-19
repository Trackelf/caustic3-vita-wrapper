#include "audio.h"
#include "libloader.h"
#include <psp2/kernel/processmgr.h>
#include <psp2/io/dirent.h>
#include <psp2/io/fcntl.h>
#include <psp2/io/stat.h>
#include <stdio.h>


#include "log.h"
unsigned int sceLibcHeapSize = 64 * 1024 * 1024;
static void write_ping(void) {
    int fd = sceIoOpen("ux0:/data/CAUSTIC3/ping.txt",
                       SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC, 0666);
    if (fd >= 0) {
        const char *txt = "ping\n";
        sceIoWrite(fd, txt, 5);
        sceIoClose(fd);
    }
}

int main() {
    sceIoMkdir("ux0:/data", 0777);
    sceIoMkdir("ux0:/data/CAUSTIC3", 0777);

    write_ping();
    debug_log("Caustic3 Vita Loader startet...");
    Loader_EarlyInit();
    // Dummy Calls
    CausticNative_SetRootPath("ux0:/data/CAUSTIC3/");
    CausticRenderer_nativeInitGraphics(960, 544);
    audio_test_silent();

    debug_log("Initialisierung abgeschlossen.");
    sceKernelExitProcess(0);
    return 0;
}