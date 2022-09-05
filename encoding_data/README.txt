
Xtensa Instruction Encoding Generator
=====================================

The tools in this directory product JSON files describing instruction
encodings based on the binutils overlay file xtensa-modules.c.  The
input format accepted is a .tar.gz or .tar.bz2 overlay tarball as
accepted by a crosstools-ng build.  Just run "make" in this directory
to produce a JSON file for each machine description.  (It takes
surprisingly long to compile these files, so feel free to build in
parallel to speed the process!).

JSON Data Format
----------------

The output JSON file consists of a single object with an "opcodes"
member, whose value is an array of opcode objects:

* opcodes[].opcode:

  An ASCII string name for the opcode, e.g. "add" or
  "ae_muls32f48p16s.ll".  These names are as they appear in the
  binutils code, so I believe they are intended to correspond to the
  strings accepted by the resulting assembler.

* opcodes[].variants[]:

  An array of "variant" encodings that can express the opcode in an
  instruction stream.

* opcodes[].variants[].format:

  An ASCII string name for the resulting instruction format, for
  example "x24" (a "normal" 3-byte Xtensa instruction), "x16a" (one of
  the two "narrow" encodings), or "ae_format2" (a HiFi 4 bundle type).

* opcodes[].variants[].slot:

  Some instruction formats are VLIW "bundles" that can encode more
  than one instruction.  These are distinguished by an integer "slot"
  ID.

* opcodes[].variants[].format_bits[]:

  An array of integers representing bits in the resulting instruction
  that must be set to one to correctly tag the format in use.  The
  bits order is little endian, so e.g. a "0" in the list indicates
  that the lowest bit of the first byte of the instruction should be
  set, a "17" will set the second lowest bit of the third byte, etc...
  There is unfortunately no representation in the input data of bits
  that must be set to zero, those are left implicit.

* opcodes[].variants[].opcode_bits[]:

  Similar to format_bits, these are bits in the instruction that must
  be set to correctly tag the specific opcode.  In the case of VLIW
  HiFi bundles, these obviously specify only bits used by the given
  slot.  Note that the input data is NOT DISJOINT, some opcode records
  have the same bit present in both opcode_bits and format_bits.

* opcodes[].variants[].args[]:

  Each opcode has an array of arguments, each a JSON object.

* opcodes[].variants[].args[].arg:

  An ASCII string name for the argument, e.g. "art" or "ae_mul_q0".
  These names seem to correlate fairly well to printed documentation,
  but in some cases the naming is obtuse.

* opcodes[].variants[].args[].field_bits:

  A list of integers representing the bit positions in the output
  instruction of each bit of the argument value.  Arguments can have
  variable size, e.g. 4 bits can store a register ID, but there are
  immediates that can be larger, and there are some ("invisible")
  arguments in the input data which are described via a zero-length
  field argument.  The bit encoding is little endian as before, that
  is the first integer stores the LE bit position within the
  instruction in which the lowest bit of the argument value should be
  placed.  Note that these bit positions are NOT IN GENERAL
  CONTIGUOUS, NOR IN-ORDER.  Especially some HiFi instructions have
  odd, disjoint encodings.  Be careful.
