// SPDX-License-Identifier: BSD-3-Clause
// Copyright(c) 2022 Google LLC. All rights reserved.
// Author: Andy Ross <andyross@google.com>

#include "xt-insn-mod.h"
#include <stdio.h>
#include <stdbool.h>

// Kludgey JSON generator for Xtensa instruction data.  Works by
// iteratively encoding data using the utilities provided in the
// binutils overlay files to extract bit positions one at a time.
// Note that a core assumption here is that the fields and operand
// encodings in the ISA are all encoded bitwise, where the only
// transformations being done are motion of individual bits.  That is
// true empyrically for all existing instruction subsets, but future
// instruction encodings might get more complicated and this code
// would be surprised.

int find_format(int slot)
{
	for (int f = 0; f < xtensa_modules.n_formats; f++) {
		for (int s = 0; s < xtensa_modules.formats[f].n_slots; s++) {
			if (xtensa_modules.formats[f].slots[s] == slot) {
				return f;
			}
		}
	}
	return -1;
}

void clear_insn(xtensa_insnbuf insn)
{
	insn[0] = insn[1] = insn[2] = insn[3] = 0;
}

int find_set_bits(const xtensa_insnbuf insn, unsigned char setbits[MAX_INSN_BITS])
{
	int n = 0;
	for (int b = 0; b < MAX_INSN_BITS; b++) {
		if (insn[b / 32] & (1 << (b % 32))) {
			setbits[n++] = b;
		}
	}
	return n;
}

int find_arg_bits(xtensa_set_field_fn setter, set_slot_fn slotter,
		  unsigned char setbits[MAX_INSN_BITS])
{
	unsigned char bits2[MAX_INSN_BITS];
	xtensa_insnbuf insn, insn2;
	int n = 0;

	for (n = 0; n < MAX_INSN_BITS; n++) {
		clear_insn(insn);
		clear_insn(insn2);
		setter(insn, (1 << n));
		slotter(insn2, insn);

		if (find_set_bits(insn2, bits2) == 0) {
			break;
		}
		setbits[n] = bits2[0];
	}

	return n;
}

void print_bits(int n, unsigned char bits[MAX_INSN_BITS])
{
	for (int i = 0; i < n; i++) {
		if (i != 0) {
			printf(",");
		}
		printf(" %d", bits[i]);
	}
}

void opcode_variant(xtensa_opcode_encode_fn encoder, int slot,
		    xtensa_iclass_internal *iclass)
{
	xtensa_slot_internal *s = &xtensa_modules.slots[slot];
	int fid = find_format(slot);

	printf("\n   { \"format\" : \"%s\", \"slot\" : %d,\n", s->format, s->slot);

	unsigned char bits[MAX_INSN_BITS];
	xtensa_insnbuf insn, insn2;
	int n_bits;

	/* Get the bits needed for the format as a whole first */
	clear_insn(insn);
	xtensa_modules.formats[fid].encode(insn);
	n_bits = find_set_bits(insn, bits);

	printf("     \"format_bits\" : [");
	print_bits(n_bits, bits);
	printf(" ],\n");

	/* Now the bits for the opcode encoding.  This happens
	 * (somewhat inexplicably) in two parts.  First step encodes
	 * the single instruction into the low bits of the the
	 * instruction word?
	 */
	clear_insn(insn);
	encoder(insn);

	/* Second step needs to move that to the slot's position in
	 * the full word?
	 */
	clear_insn(insn2);
	s->set_slot(insn2, insn);

	n_bits = find_set_bits(insn2, bits);
	printf("     \"opcode_bits\" : [");
	print_bits(n_bits, bits);
	printf(" ],\n");

	printf("     \"args\" : [");
	for (int i = 0; i < iclass->n_operands; i++) {
		int opid = iclass->operands[i].id[0];
		xtensa_operand_internal *op = &xtensa_modules.operands[opid];

		n_bits = find_arg_bits(s->field_setters[op->field],
				       s->set_slot, bits);

		if (i != 0) {
			printf(",");
		}
		printf("\n       { \"arg\" : \"%s\",\n", op->name);
		printf("         \"field_bits\" : [");
		print_bits(n_bits, bits);
		printf(" ] }");
	}
	printf(" ] }");
}

int main(void)
{
	printf("{ \"opcodes\" : [");
	for (int i = 0; i < xtensa_modules.n_opcodes; i++) {
		if (i != 0) {
			printf(",");
		}
		printf("\n { \"opcode\" : \"%s\", \"variants\" : [",
		       xtensa_modules.opcodes[i].name);

		bool first = true;
		for (int s = 0; s < xtensa_modules.n_slots; s++) {
			void *encoder = xtensa_modules.opcodes[i].encoders[s];
			if(encoder) {
				if (!first) {
					printf(",");
				}
				first = false;

				int icid = xtensa_modules.opcodes[i].iclass;
				void *iclass = &xtensa_modules.iclasses[icid];

				opcode_variant(encoder, s, iclass);
			}
		}
		printf(" ] }");
	}
	printf("] }\n");
}
