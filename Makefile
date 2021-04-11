CC=gcc
CFLAGS=-Wvla -Wall -Werror -std=gnu11 -Os -s -ffunction-sections -fdata-sections -fno-asynchronous-unwind-tables -fno-stack-protector
LDFLAGS=-Wl,--gc-sections,-z,norelro,--hash-style=sysv,--build-id=none -nostartfiles

STRIP=strip -S --strip-unneeded -R .comment -R .note -R .note.ABI-tag -R .eh_frame -R .eh_frame_hdr -R .note.gnu.property -R .gnu.version -R .hash -R .data

SRCDIR=src
LIBDIR=lib
BUILDDIR=build

LIBS=-I$(LIBDIR)

TESTDIR=tests
TESTASM=$(wildcard $(TESTDIR)/*.asm)
TESTBIN=$(TESTASM:.asm=.x2017)

# fill in all your make rules
all: vm_x2017 objdump_x2017

.PHONY: all tests run_tests clean

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

vm_x2017: $(BUILDDIR)/vm_x2017.o $(BUILDDIR)/parser.o $(BUILDDIR)/start.o
	$(CC) $(LDFLAGS) -o $@ $^
	$(STRIP) $@

objdump_x2017: $(BUILDDIR)/objdump_x2017.o $(BUILDDIR)/parser.o $(BUILDDIR)/start.o
	$(CC) $(LDFLAGS) -o $@ $^
	$(STRIP) $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.c $(LIBDIR)/%.h $(LIBDIR)/parser.h | $(BUILDDIR)
	$(CC) $(CFLAGS) $(LIBS) -c -o $@ $<

tests: $(TESTBIN)

$(TESTDIR)/%.x2017: $(TESTDIR)/%.asm
	./assembler.py $^ $@

run_tests:
	./test.sh

clean:
	rm -f vm_x2017 objdump_x2017
	rm -f $(BUILDDIR)/*.o
	rmdir $(BUILDDIR)
