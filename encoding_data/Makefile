# SPDX-License-Identifier: BSD-3-Clause
# Copyright(c) 2022 Google LLC. All rights reserved.
# Author: Andy Ross <andyross@google.com>

OVERLAY_DIR = ..

# Modules file within tarball
MODFILE = binutils/bfd/xtensa-modules.c

CC = gcc

# Don't optimize!  It adds FAR longer (~4s) to the build time for
# these huge files than the runtime savings (~10ms)
CFLAGS = -std=c11 -Wall -Wno-unused-variable

JSONS := $(patsubst %.tar,%.json,$(basename $(notdir $(wildcard $(OVERLAY_DIR)/*.tar*))))

all: $(JSONS)

clean:
	rm -f tmpmod-* insgen-* *.json

%.json : gen.%

# Two kinds of tarball, and the path to the file can be absolute or
# prefixed by a "./".  Clumsy.

tmpmod-%.c : $(OVERLAY_DIR)/%.tar.gz
	(tar -O -x -f $< $(MODFILE) || tar -O -x -f $< ./$(MODFILE)) \
	 | grep -vP '^#\s*include' > $@

tmpmod-%.c : $(OVERLAY_DIR)/%.tar.bz2
	(tar -O -x -f $< $(MODFILE) || tar -O -x -f $< ./$(MODFILE)) \
	 | grep -vP '^#\s*include' > $@

tmpmod-%.o : tmpmod-%.c
	$(CC) $(CFLAGS) -include xt-insn-mod.h -c -o $@ $<

insgen-% : tmpmod-%.o xt-insn-gen.c
	$(CC) $(CFLAGS) -o $@ $^

%.json : insgen-%
	./$< > $@
	./validate.py $@
