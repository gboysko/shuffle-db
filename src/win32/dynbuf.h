#ifndef __DYNBUF_H__
#define __DYNBUF_H__

typedef struct __dynbuf_t {
  int size,pos;
  void *data;
  int granularity;
} dynbuf_t;

dynbuf_t* dynbuf_create(int granularity);
int dynbuf_append(dynbuf_t *db, void* data, int size);
void* dynbuf_get(dynbuf_t *db, int handle);
#define dynbuf_free(db) do { dynbuf_destroy(db); (db)=NULL; } while(0)
void dynbuf_destroy(dynbuf_t *db);

#endif//__DYNBUF_H__
