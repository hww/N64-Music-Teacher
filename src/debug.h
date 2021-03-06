/*****************************************************************************
* @project N64 Music Teacher
* @info The game for learning music.
* @platform Nintendo 64
* @autor Valery P. (https://github.com/hww)
*****************************************************************************/

extern char __str[128];
extern char __strbuffer[512];
extern void __PrintStr (char* str);
extern void EndPrintf (void);
extern void InitDebug (void);

#define Print(x...) ({ sprintf(__str, x); __PrintStr(__str); })
#define EPrint(x...) ({ sprintf(__str, x); __PrintStr(__str); EndPrintf(); })
