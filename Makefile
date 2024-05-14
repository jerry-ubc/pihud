CC=gcc
CFLAGS=-Iinclude
OBJDIR=obj
BINDIR=bin

$(shell mkdir -p $(OBJDIR) $(BINDIR))

# Object files
_OBJS=main.o config.o led.o
OBJS=$(patsubst %,$(OBJDIR)/%,$(_OBJS))

# Include directories
_DEPS = config.h led.h
DEPS = $(patsubst %,include/%,$(_DEPS))

$(OBJDIR)/%.o: src/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BINDIR)/hud: $(OBJS)
	$(CC) -o $@ $^

all: $(BINDIR)/hud

.PHONY: clean

clean:
	rm -f $(OBJDIR)/*.o $(BINDIR)/hud
