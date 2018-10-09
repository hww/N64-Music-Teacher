/*****************************************************************************
* @project N64 Music Teacher
* @info The game for learning music.
* @platform Nintendo 64
* @autor Valery P. (https://github.com/hww)
*****************************************************************************/

#ifndef LIB64DAT_H
#define LIB64DAT_H

#define SEEK_SET    0  /*  Seek relative to start of file  */
#define SEEK_CUR    1  /*  Seek relative to current positn */
#define SEEK_END    2  /*  Seek relative to end of file    */
#define FILENAME_SZ 64

#define EOF -1

typedef struct file_data_s
{
    char filename[FILENAME_SZ];
    unsigned long offset;
    unsigned long length;
}file_data;

typedef struct node_s
{
    struct node_s *next;
    file_data data;
}node;

typedef struct file_lib_s
{
    short version;
    unsigned long number;
    node *head;
    unsigned long data_pos;
}file_lib;


typedef struct FILE_s
{
    char mode;
    unsigned long length;
    unsigned long basep;
    unsigned long currp;
    unsigned char *buf;
    file_lib *flib;
    char buf_off;
}FILE;

extern int backwards;

#endif
