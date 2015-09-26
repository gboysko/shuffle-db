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
#include <ctype.h>
#include <time.h>
#include <errno.h>

#define TITLE "KeyJ's iPod shuffle Database Builder"
#define VERSION "0.5"

#include "direct.h" // lcc-win32 hack

#include "dynbuf.h"
#include "browse.h"

int do_wait=1;

int total_count=0;
FILE *iTunesSD;
#define HEADER_SIZE 18
uint8_t header[HEADER_SIZE];
#define ENTRY_SIZE 558
uint8_t entry[ENTRY_SIZE];
#define ENTRY_START 32

#define streq(a,b) (!strcmp(a,b))

char *get_ext(char *filename) {
  static char ext[8];
  char *dot=strrchr(filename,'.');
  int i,l;
  ext[0]='\0';
  if(!dot) return ext;
  l=strlen(++dot)+1;
  if(l>8) return ext;
  for(i=0; i<l; ++i)
    ext[i]=tolower(dot[i]);
  return ext;
}


void do_exit(int code) {
  if(do_wait) {
    printf("\nPress ENTER to quit ...\n");
    getchar();
  }
  exit(code);
}


void write_to_db(char *filename) {
  int offset;
  char *ext;
  if(!filename || !filename[0]) return;
  ext=get_ext(filename);
  if(streq(ext,"mp3"))
    entry[29]=1;
  else if(streq(ext,"m4a") || streq(ext,"m4b") || streq(ext,"m4p"))
    entry[29]=2;
  else if(streq(ext,"aa"))
    entry[29]=4;
  ++total_count;
  ++filename;
  for(offset=ENTRY_START; offset<ENTRY_SIZE; offset+=2) {
    entry[offset]=0;
    if(*filename)
      entry[offset+1]=*filename++;
    else
      entry[offset+1]=0;
  }
  if(strcmp(ext,"m4b") && strcmp(ext,"aa"))
    entry[555]=1;
  fwrite(entry,1,ENTRY_SIZE,iTunesSD);
}


int idcmp(const void* a, const void* b) {
  return stricmp(*(char**)a,*(char**)b);
}


void browse(char *path) {
#define PATH_LENGTH 256
  void *bh;
  char *filename; int isdir;
  char temp[PATH_LENGTH+8];
  dynbuf_t *names,*indexes,*pointers;
  int i,count=0,mp3count=0;

  if(path[0]=='.' && !path[1])
    printf("%c: ",path_delim);
  else
    printf("%s: ",path+1);
  fflush(stdout);

  bh=browse_start(path);
  if(!bh) {
    printf("cannot browse\n",path+1);
    return;
  }

  names=dynbuf_create(262144);
  indexes=dynbuf_create(1024);
  pointers=dynbuf_create(1024);

  while(browse_next(bh,&filename,&isdir)) {
    int index;
    if(filename[0]=='.') continue;

    if(!isdir) {
      char *ext=get_ext(filename);
      if(strcmp(ext,"mp3") && strcmp(ext,"wav") && strcmp(ext,"aa")
      && strcmp(ext,"m4a") && strcmp(ext,"m4b") && strcmp(ext,"m4p"))
        continue;
    }

    ++count;
    if(!isdir) ++mp3count;

    snprintf(temp,PATH_LENGTH,"%c%s",isdir?'D':'F',filename);
    index=dynbuf_append(names,temp,strlen(temp)+1);
    dynbuf_append(indexes,&index,sizeof(int));
  }
  browse_end(bh);
  printf("%d files\n",mp3count);

  for(i=0; i<count; ++i) {
    int *index=dynbuf_get(indexes,i*sizeof(int));
    char* ptr=dynbuf_get(names,*index);
    dynbuf_append(pointers,&ptr,sizeof(char*));
  }
  dynbuf_free(indexes);
  qsort(pointers->data,count,sizeof(char*),idcmp);

  for(i=0; i<count; ++i) {
    filename=*(char**)dynbuf_get(pointers,i*sizeof(char*));
    snprintf(temp,PATH_LENGTH,"%s%c%s",path,path_delim,filename+1);
    if(filename[0]=='D')
      browse(temp);
    else
      write_to_db(temp);
  }

  dynbuf_free(names);
  dynbuf_free(pointers);
}


int kill(char *filename, char *title) {
  printf("Resetting %s ... ",title);
  if(!remove(filename))
    printf("OK.\n");
  else {
    if(errno==2)
      printf("not needed.\n");
    else {
      printf("FAILED.\n");
      return 0;
    }
  }
  return 1;
}


int main(int argc, char *argv[]) {
  int have=0;
  printf("Welcome to " TITLE ", version " VERSION "\n\n");

  if(argc==2 && !strcmp(argv[1],"-q")) do_wait=0;

  if(chdir("iPod_Control/iTunes") || chdir("../..")) {
    printf("ERROR: No iPod control directory found!\n"
           "Please make sure that:\n"
           " (*) this program's working directory is the iPod's root directory\n"
           " (*) the iPod was correctly initialized with iTunes\n");
    do_exit(1);
  }

  iTunesSD=fopen("iPod_Control/iTunes/iTunesSD","rb");
  if(iTunesSD) {
    if(fread(header,HEADER_SIZE,1,iTunesSD)) have=1;
    if(have && fread(entry,ENTRY_SIZE,1,iTunesSD)) have=2;
    fclose(iTunesSD);
  }

  if(have<1) {
    printf("Rebuilding iTunesSD file header from scratch.\n");
    memset(header,0,HEADER_SIZE);
    header[3]=0x01;
    header[4]=0x06;
    header[8]=0x12;
  } else
    printf("Using iTunesSD file header from existing database.\n");

  if(have<2) {
    printf("Rebuilding iTunesSD entry header from scratch.\n");
    memset(entry,0,ENTRY_SIZE);
    entry[1]=0x02;
    entry[2]=0x2E;
    entry[3]=0x5A;
    entry[4]=0xA5;
    entry[5]=0x01;
    entry[26]=0x64;
    entry[29]=0x01;
    entry[31]=0x02;
  } else
    printf("Using iTunesSD entry header from existing database.\n");

  iTunesSD=fopen("iPod_Control/iTunes/iTunesSD","wb");
  if(!iTunesSD) {
    printf("ERROR: Cannot write to the iPod database file (iTunesSD)!\n"
           "Please make sure that:\n"
           " (*) you have sufficient permissions to write to the iPod volume\n"
           " (*) you are actually using an iPod shuffle, and not some other iPod model :)\n");
    do_exit(1);
  }
  fwrite(header,1,HEADER_SIZE,iTunesSD);

  printf("\nSearching for files on the iPod.\n");
  browse(".");
  printf("%d playable files were found on your iPod.\n\n",total_count);

  printf("Fixing iTunesSD header.\n");
  header[2]=total_count&0xFF;
  header[1]=total_count>>8;
  fseek(iTunesSD,0,SEEK_SET);
  fwrite(header,1,HEADER_SIZE,iTunesSD);

  if(kill("iPod_Control/iTunes/iTunesPState", "playback state")
    *kill("iPod_Control/iTunes/iTunesStats",  "statistics")
    *kill("iPod_Control/iTunes/iTunesShuffle","shuffle sequence"))
    printf("\nThe iPod shuffle database was rebuilt successfully."
           "\nHave fun listening to your music!\n");
  else
    printf("\nWARNING: The main database file was rebuild successfully, but there were errors"
           "\n         while resetting the other files. However, playback MAY work correctly.\n");

  do_exit(0);
  return 0;
}
