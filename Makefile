TARGET = fst
BUILDDIR := build

DEPS = gio-2.0
SRCDIR = src
PARSERDIR = parsers
INCDIR = include

CC := gcc
CFLAGS := -Wall -Wextra $(shell pkg-config --cflags $(DEPS)) -Iinclude
LDFLAGS := $(shell pkg-config --libs $(DEPS))


SRCS = $(wildcard $(SRCDIR)/*.c) $(wildcard $(PARSERDIR)/*.c)
OBJS = $(addprefix $(BUILDDIR)/, $(notdir $(SRCS:.c=.o)))

all: $(BUILDDIR)/$(TARGET)
$(BUILDDIR)/$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/%.o: $(PARSERDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)

.PHONY: all clean
