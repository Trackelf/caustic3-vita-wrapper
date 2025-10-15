#include <psp2/kernel/modulemgr.h>
#include <psp2/kernel/error.h>
#include <psp2/io/fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "soloader.h"

int soloader_load_library(const char *path) {
	SceUID modid = sceKernelLoadStartModule(path, 0, NULL, 0, NULL, NULL);
	return modid < 0 ? -1 : 0;
}
