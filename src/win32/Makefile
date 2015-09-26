CC=lcc.exe
LD=lcclnk.exe
CFLAGS=-A
LDFLAGS=-s -subsystem console

OBJS=	main.obj dynbuf.obj browse_win32.obj
LIBS=
TARGET=rebuild_db.exe

all:	$(TARGET)

test:	$(TARGET)
	$(TARGET) -q

$(TARGET):	$(OBJS)
	$(LD) $(LDFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

.c.obj:
	$(CC) $(CFLAGS) -c $*.c

clean:
	del *.obj
	del $(TARGET)
