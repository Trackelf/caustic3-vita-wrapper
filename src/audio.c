#include <psp2/audioout.h>
#include <psp2/kernel/threadmgr.h>
#include "log.h"

void audio_test_silent(void) {
    int port = sceAudioOutOpenPort(SCE_AUDIO_OUT_PORT_TYPE_MAIN, 1024, 48000, SCE_AUDIO_OUT_MODE_STEREO);
    if (port < 0) { debug_log("audio: open failed"); return; }
    int vol[2] = { 0x8000, 0x8000 };
    sceAudioOutSetVolume(port, SCE_AUDIO_VOLUME_FLAG_L_CH | SCE_AUDIO_VOLUME_FLAG_R_CH, vol);
    static short zeros[1024*2] = {0};
    for (int i = 0; i < 30; ++i) {
        sceAudioOutOutput(port, zeros);
        sceKernelDelayThread(16666);
    }
    sceAudioOutReleasePort(port);
    debug_log("audio: silent test done");
}
