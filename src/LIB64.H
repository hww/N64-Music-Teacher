#ifndef LIB64_H
#define LIB64_H

#include "lib64dat.h"

extern file_lib *init_filelib(u32 _filesegment_start);

extern int deinit_filelib(file_lib *lib);

extern node *libfile_exist(file_lib *lib, char *filename);

extern FILE *fopen(char *filename, char *mode);

extern int fclose(FILE *stream);

extern unsigned fread(void *ptr, unsigned size, unsigned n, FILE *stream);

extern int fgetc(FILE *stream);

extern int fseek(FILE *stream, unsigned long offset, int whence);

extern long ftell(FILE *stream);

extern int feof(FILE *stream);

#endif
