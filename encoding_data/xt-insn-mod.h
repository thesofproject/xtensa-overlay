// SPDX-License-Identifier: BSD-3-Clause
// Copyright(c) 2022 Google LLC. All rights reserved.
// Author: Andy Ross <andyross@google.com>

// Minimal redeclaration, slightly reverse-engineered, of the
// interfaces used by Xtensa xtensa-module.c files (ones compatible
// with 2.39 or so, anyway).  The full interface is somewhat thicker
// and part of upstream binutils.

#ifndef _XT_INSN_MOD_H
#define _XT_INSN_MOD_H

// Biggest HiFi 5 bundle is 11 bytes
#define MAX_INSN_BITS 88

typedef unsigned int uint32;
typedef unsigned int xtensa_insnbuf[(MAX_INSN_BITS + 31)/32];

typedef struct {
	const char *name;
	int unused0;
	int unused1;
} xtensa_sysreg_internal, xtensa_state_internal;

typedef struct {
	const char *name;
	int unused0;
} xtensa_funcUnit_internal;

typedef struct {
	int id;
	int unused0;
} xtensa_funcUnit_use;

typedef struct {
	const char *name;
	const char *unused0;
	int id;
	int unused1;
	int unused2;
} xtensa_regfile_internal;

typedef struct {
	const char *name;
	int unused0;
	int unused1;
	int unused2;
	char io;
} xtensa_interface_internal;

typedef struct {
	int id[1];
	char io;
} xtensa_arg_internal;

typedef struct {
	int n_operands;
	xtensa_arg_internal *operands;
	int n_states;
	xtensa_arg_internal *states;
	int n_interfaces;
	int *interfaces;
} xtensa_iclass_internal;

typedef struct {
	const char *name;
	int field;
	int unused0;
	int unused1;
	int unused2;
	void *encode_fn;
	void *decode_fn;
	void *ator_fn;
	void *rtoa_fn;
} xtensa_operand_internal;

typedef void (*xtensa_opcode_encode_fn)(xtensa_insnbuf);
typedef int (*xtensa_opcode_decode_fn)(const xtensa_insnbuf);
typedef unsigned (*xtensa_get_field_fn)(const xtensa_insnbuf);
typedef void (*xtensa_set_field_fn)(xtensa_insnbuf, unsigned int);

typedef void (*get_slot_fn) (const xtensa_insnbuf, xtensa_insnbuf);
typedef void (*set_slot_fn) (xtensa_insnbuf, const xtensa_insnbuf);

typedef struct {
	const char *name;
	int iclass;
	int unused0;
	xtensa_opcode_encode_fn *encoders;
	int unused1;
	void *unused2;
} xtensa_opcode_internal;

typedef struct {
	const char *name;
	const char *format;
	int slot;
	get_slot_fn get_slot;
	set_slot_fn set_slot;
	xtensa_get_field_fn *field_getters;
	xtensa_set_field_fn *field_setters;
	xtensa_opcode_decode_fn decoder;
	const char *unused0;
} xtensa_slot_internal;

typedef struct {
	const char *name;
	int unused0;
	xtensa_opcode_encode_fn encode;
	int n_slots;
	int *slots;
} xtensa_format_internal;

typedef struct {
	int bigendian;
	int insn_size;
	int unused0;
	int n_formats;
	xtensa_format_internal *formats;
	int (*format_decoder)(const xtensa_insnbuf);
	int (*length_decoder)(const unsigned char *);
	int n_slots;
	xtensa_slot_internal *slots;
	int n_fields;
	int n_operands;
	xtensa_operand_internal *operands;
	int n_iclasses;
	xtensa_iclass_internal *iclasses;
	int n_opcodes;
	xtensa_opcode_internal *opcodes;
	void *unused1;
	int n_regfiles;
	void *unused2;
	int n_states;
	void *states;
	void *unused3;
	int n_sysregs;
	void *sysregs;
	void *unused4;
	int sysreg_max[2];
	int unused5[2];
	int n_interfaces;
	xtensa_interface_internal *interfaces;
	void *unused6;
	int n_units;
	void *units;
	void *unused7;
} xtensa_isa_internal;

#define ATTRIBUTE_UNUSED /**/

#define XTENSA_OPERAND_IS_REGISTER       1
#define XTENSA_OPERAND_IS_PCRELATIVE     2
#define XTENSA_OPERAND_IS_INVISIBLE      4
#define XTENSA_OPERAND_IS_UNKNOWN        8
#define XTENSA_OPCODE_IS_BRANCH          1
#define XTENSA_OPCODE_IS_JUMP            2
#define XTENSA_OPCODE_IS_LOOP            4
#define XTENSA_OPCODE_IS_CALL            8
#define XTENSA_STATE_IS_EXPORTED         1
#define XTENSA_STATE_IS_SHARED_OR        2
#define XTENSA_INTERFACE_HAS_SIDE_EFFECT 1
#define XTENSA_UNDEFINED -1

typedef int xtensa_interface;

extern xtensa_isa_internal xtensa_modules;

#endif /* _XT_INSN_MOD_H */
