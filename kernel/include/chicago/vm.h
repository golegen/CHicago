// File author is Ítalo Lima Marconato Matias
//
// Created on October 21 of 2018, at 18:36 BRT
// Last edited on October 21 of 2018, at 18:37 BRT

#ifndef __CHICAGO_VM_H__
#define __CHICAGO_VM_H__

#define VM_OBJECT_TYPE_INT 0x00
#define VM_OBJECT_TYPE_UINT 0x01
#define VM_OBJECT_TYPE_FLOAT 0x02

#define VM_OPCODE_ADD 0x00
#define VM_OPCODE_SUB 0x01
#define VM_OPCODE_MUL 0x02
#define VM_OPCODE_DIV 0x03
#define VM_OPCODE_MOD 0x04
#define VM_OPCODE_SHL 0x05
#define VM_OPCODE_SHR 0x06
#define VM_OPCODE_AND 0x07
#define VM_OPCODE_OR 0x08
#define VM_OPCODE_XOR 0x09
#define VM_OPCODE_LDS 0x0A
#define VM_OPCODE_LDU 0x0B
#define VM_OPCODE_LDF 0x0C
#define VM_OPCODE_PRINT 0x0D

typedef struct {
	UInt8 type;
	union {
		UInt32 uval;
		Int32 ival;
		Float fval;
	};
} VmObject, *PVmObject;

typedef struct {
	UInt8 opcode;
	VmObject arg;
} VmInstruction, *PVmInstruction;

typedef struct {
	List body;
} VmModule, *PVmModule;

PVmModule VmLoadModule(PChar filename);
PVmObject VmRunModule(PVmModule module);

#endif		// __CHICAGO_VM_H__
