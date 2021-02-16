SHELL   = /bin/sh

TARGET  = openrelative
SOURCES = $(shell echo *.c)
OBJECTS = $(SOURCES:.c=.o)
LIBRARY = $(OBJECTS:.o=.so)
CFLAGS += $(shell pkg-config --cflags --libs  geany)
CFLAGS += -W
CFLAGS += -Wall
CFLAGS += -O2
PREFIX  = $(DESTDIR)/usr/local
BINDIR  = $(PREFIX)/lib/geany

$(LIBRARY): $(SOURCES)
	$(CC) -c $(SOURCES) $(CFLAGS) -fPIC
	$(CC) -shared -o $(LIBRARY) $(OBJECTS);

install:
	echo "installing"
	if [ -d "/usr/lib/geany" ]; then cp $(LIBRARY) /usr/lib/geany/;	fi
	if [ -d "/usr/lib/x86_64-linux-gnu/geany/" ]; then cp $(LIBRARY) /usr/lib/x86_64-linux-gnu/geany/; fi

#uninstall:
	#@rm -f $(BINDIR)/$(LIBRARY)

.PHONY : clean
clean :
	@rm -f $(OBJECTS)
	@rm -f $(LIBRARY)
