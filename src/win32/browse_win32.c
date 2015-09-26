/*
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
#include "browse.h"

char path_delim='/';

typedef struct {
  HANDLE hFind;
  WIN32_FIND_DATA sr;
  int first;
} browsedata;


void* browse_start(char* path) {
  browsedata *h;
  char fpath[MAX_PATH+8];
  int i;

  h=calloc(1,sizeof(browsedata));
  if(!h) return NULL;

  strncpy(fpath,path,MAX_PATH);
  fpath[MAX_PATH]='\0';
  i=strlen(fpath)-1;
  if(fpath[i]==path_delim) fpath[i]='\0';
  strcat(fpath,"\\*.*");

  h->hFind=FindFirstFile(fpath,&h->sr);
  if(h->hFind==INVALID_HANDLE_VALUE) {
    free(h);
    return NULL;
  }
  h->first=1;

  return h;
}


int browse_next(void* handle, char **name, int *isdir) {
  browsedata *h=(browsedata*)handle;
  if(!h) return 0;
  if(!h->first && !FindNextFile(h->hFind,&h->sr)) return 0;
  *name=h->sr.cFileName;
  *isdir=h->sr.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY;
  h->first=0;
  return 1;
}


void browse_end(void* handle) {
  browsedata *h=(browsedata*)handle;
  if(!h) return;
  FindClose(h->hFind);
  free(h);
}
