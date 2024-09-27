#!/usr/bin/env python3

import sys
import json
import pprint
import types

js = json.load(open(sys.argv[1]),
               object_hook=lambda d: types.SimpleNamespace(**d))

#pprint.pp(js.opcodes)

# Map of format -> encoded bits
format_bits = {}

# Sets of bits indexed by (format,slot) tuple
slot_bits = {}

# Format to count of slots
format_slots = {}

# First pass to initialize dicts
for op in js.opcodes:
    for v in op.variants:
        format_bits[v.format] = v.format_bits
        format_slots[v.format] = -1
        slot_bits[(v.format, v.slot)] = set()

# Second to compute them
for op in js.opcodes:
    for v in op.variants:
        for a in v.args:
            format_slots[v.format] = max([format_slots[v.format], v.slot + 1])
            for b in a.field_bits:
                slot_bits[(v.format, v.slot)].add(b)

# Make sure all instances of a given format have the same bit representation
for op in js.opcodes:
    for v in op.variants:
        assert v.format_bits == format_bits[v.format]

# Make sure slots don't overlap with each other
for f in format_bits:
    for s1 in range(format_slots[f]):
        for s2 in range(s1 + 1, format_slots[f]):
            b1 = slot_bits[(f, s1)]
            b2 = slot_bits[(f, s2)]
            assert b1.intersection(b2) == set()

# Make sure format bits don't overlap with slots.  This is incomplete,
# as the data from xtensa-modules.c only records one bits.  Bits
# required to be zero are effectively invisible, they look like
# "holes" to us.
for f in format_bits:
    for s in range(format_slots[f]):
        sb = slot_bits[(f, s)]
        for fb in format_bits[f]:
            assert fb not in sb
