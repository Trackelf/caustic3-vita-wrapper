
#ifndef LIBLOADER_H
#define LIBLOADER_H
int Loader_EarlyInit(void);


void CausticNative_SetRootPath(const char *path);
void CausticRenderer_nativeInitGraphics(int width, int height);

#endif
