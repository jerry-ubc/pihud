CC=gcc
CFLAGS=-Iinclude -Ilibssd1306/include
LDFLAGS=-Llibssd1306/src/.libs -lssd1306_i2c
LDFLAGS=-Llibssd1306/src/.libs -lssd1306_i2c -Wl,-rpath,libssd1306/src/.libs
OBJDIR=obj
BINDIR=bin

$(shell mkdir -p $(OBJDIR) $(BINDIR))

#object files
_OBJS=main.o config.o led.o
OBJS=$(patsubst %,$(OBJDIR)/%,$(_OBJS))

#include directories
_DEPS = config.h led.h
DEPS = $(patsubst %,include/%,$(_DEPS))

#pattern rule for compiling .c to .o
$(OBJDIR)/%.o: src/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

#link program
$(BINDIR)/hud: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

#default target
all: $(BINDIR)/hud

.PHONY: clean
clean:
	rm -f $(OBJDIR)/*.o $(BINDIR)/hud

# If the SSD1306 library needs to be compiled from your project
.PHONY: libssd1306
libssd1306:
	$(MAKE) -C libssd1306