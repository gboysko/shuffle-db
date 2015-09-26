#ifndef __BROWSE_H__
#define __BROWSE_H__

extern char path_delim;

void* browse_start(char* path);
int browse_next(void* handle, char **name, int *isdir);
void browse_end(void* handle);

#endif//__BROWSE_H__
