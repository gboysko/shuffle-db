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

#include "dynbuf.h"


dynbuf_t* dynbuf_create(int granularity) {
  dynbuf_t *db=calloc(1,sizeof(dynbuf_t));
  if(!db) return NULL;
  db->granularity=granularity;
  return db;
}


void dynbuf_destroy(dynbuf_t *db) {
  if(!db) return;
  if(db->data) free(db->data);
  free(db);
}


void* dynbuf_get(dynbuf_t *db, int handle) {
  if(!db || !db->data || handle>db->pos) return NULL;
  return &((uint8_t*)db->data)[handle];
}


int dynbuf_append(dynbuf_t *db, void* data, int size) {
  int start;
  if(!db) return -1;
  start=db->pos;
  if((start+size)>db->size) {
    db->size=((start+size+db->granularity-1)/db->granularity)*db->granularity;
    db->data=realloc(db->data,db->size);
    if(!db->data) {
      db->size=0;
      return 0;
    }
  }
  memcpy(&((uint8_t*)db->data)[start],data,size);
  db->pos+=size;
  return start;
}
