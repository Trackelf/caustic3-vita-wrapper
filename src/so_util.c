
#include "so_util.h"
#include <string.h>
#include <psp2/kernel/sysmem.h>
#include <psp2/kernel/modulemgr.h>

int so_util_load_dummy(const char *path) {
	SceUID modid = sceKernelLoadStartModule(path, 0, NULL, 0, NULL, NULL);
	return modid < 0 ? -1 : modid;
}
