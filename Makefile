# This is a comment

# GIMP
GIMPTOOL=gimptool-2.0

# Compiler
CC=gcc
CFLAGS=-c -Wall
CINCLUDES=-I/usr/include/gtk-2.0 -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/lib/x86_64-linux-gnu/gtk-2.0/include -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/pango-1.0 -I/usr/include/gio-unix-2.0/ -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng12 -I/usr/include/gimp-2.0

# Linker
LD=$(CC)
LDFLAGS=
LIBS=-lgimpui-2.0 -lgimpwidgets-2.0 -lgimpmodule-2.0 -lgimp-2.0 -lgimpmath-2.0 -lgimpconfig-2.0 -lgimpcolor-2.0 -lgimpbase-2.0 -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lgio-2.0 -lpangoft2-1.0 -lpangocairo-1.0 -lgdk_pixbuf-2.0 -lcairo -lpango-1.0 -lfreetype -lfontconfig -lgobject-2.0 -lglib-2.0

# Source
HEADERS=src/error.h src/mbb/image.h src/mbb/images_pyramid.h src/mbb/math.h src/mbb/mbb.h src/mbb/prerequisites.h
SOURCES=src/main.c src/error.c src/mbb/image.c src/mbb/images_pyramid.c src/mbb/math.c src/mbb/mbb.c

# Binary
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=mbb

#.IGNORE:
# Disable echo.
.SILENT:
# Clear out all suffixes.
.SUFFIXES:
# List only those we use.
.SUFFIXES: .o .c


.c.o:
	$(CC) $(CFLAGS) $(CINCLUDES) -c $< -o $@

all: build


rebuild: clean build


build: $(EXECUTABLE) install


$(EXECUTABLE): $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@

$(OBJECTS): $(HEADERS)

clean: uninstall
	rm -f *.o src/*.o src/mbb/*.o $(EXECUTABLE)

install: $(EXECUTABLE)
	$(GIMPTOOL) --install-bin $(EXECUTABLE)

uninstall:
	$(GIMPTOOL) --uninstall-bin $(EXECUTABLE)
