CC=gcc
CFLAGS=-m32 -Wvla -Wall -Werror -std=gnu11 -Os -s -ffunction-sections -fdata-sections -flto -mpreferred-stack-boundary=2 -fno-asynchronous-unwind-tables
LDFLAGS=--gc-sections,-flto,-z,norelro,--hash-style=sysv,--build-id=none

STRIP=strip -S --strip-unneeded -R .comment -R .note -R .note.ABI-tag -R .eh_frame -R .eh_frame_hdr -R .note.gnu.property -R .gnu.version -R .hash -R .bss -R .data

# fill in all your make rules

vm_x2017: 
	echo "vm_x2017"

objdump_x2017: objdump_x2017.c objdump_x2017.h
	$(CC) $(CFLAGS) objdump_x2017.c -o $@ -Wl,$(LDFLAGS)
	$(STRIP) $@

tests:
	echo "tests"

run_tests:
	./objdump_x2017 tests/two_funcs.x2017

clean:
	rm objdump_x2017

