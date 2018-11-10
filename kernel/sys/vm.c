// File author is Ítalo Lima Marconato Matias
//
// Created on October 21 of 2018, at 18:34 BRT
// Last edited on November 10 of 2018, at 13:44 BRT

#include <chicago/console.h>
#include <chicago/file.h>
#include <chicago/list.h>
#include <chicago/mm.h>
#include <chicago/vm.h>

VmObject VmNullObject = { 0 };

static PVmObject VmPopStack(PList stack, Boolean remove) {
	if (stack == Null || ((UIntPtr)stack) >= MM_USER_END) {																		// Sanity check
		return &VmNullObject;																									// Failed, return the null object
	} else if (stack->length == 0) {																							// Empty stack?
		UIntPtr old = ConGetForeground();																						// Yes...
		
		ConSetForeground(0xFFFFFF55);
		ConWriteFormated("warning:");
		ConSetForeground(old);
		ConWriteFormated(" empty stack\r\n");
		
		return &VmNullObject;
	}
	
	return (PVmObject)(remove ? ListRemove(stack, stack->length - 1) : ListGet(stack, stack->length - 1));						// Ok, use the ListRemove function if the remove flag is set, else, use the ListGet function
}

static PVmObject VmPopCallStack(PList stack) {
	if (stack == Null || ((UIntPtr)stack) >= MM_USER_END) {																		// Sanity check
		return &VmNullObject;																									// Failed, return the null object
	} else if (stack->length == 0) {																							// Empty call stack?
		UIntPtr old = ConGetForeground();																						// Yes...
		
		ConSetForeground(0xFFFFFF55);
		ConWriteFormated("warning:");
		ConSetForeground(old);
		ConWriteFormated(" empty call stack\r\n");
		
		return &VmNullObject;
	}
	
	return (PVmObject)ListRemove(stack, stack->length - 1);																		// Ok, use the ListRemove
}

static UInt32 VmConvertValue(PVmObject obj, UInt8 new) {
	if (obj == Null) {																											// Sanity check
		return VmNullObject.uval;																								// Failed, return the null object
	}
	
	union {																														// Helper union
		UInt32 uval;
		Int32 ival;
		Float fval;
	} u;
	
	if (obj->type == VM_OBJECT_TYPE_FLOAT && new == VM_OBJECT_TYPE_INT) {														// Convert float to int
		u.ival = (Int32)obj->fval;
	} else if (obj->type == VM_OBJECT_TYPE_INT && new == VM_OBJECT_TYPE_FLOAT) {												// Convert int fo float
		u.fval = obj->ival;
	} else if (obj->type == VM_OBJECT_TYPE_FLOAT && new == VM_OBJECT_TYPE_UINT) {												// Convert float to uint
		u.uval = (UInt32)obj->fval;
	} else if (obj->type == VM_OBJECT_TYPE_UINT && new == VM_OBJECT_TYPE_FLOAT) {												// Convert uint to float
		u.fval = (Float)obj->uval;
	} else {																													// No need to convert
		u.uval = obj->uval;
	}
	
	return u.uval;
}

static Void VmBinaryOperation(PList stack, UInt8 type) {
	if (stack == Null || ((UIntPtr)stack) >= MM_USER_END) {																		// Sanity check
		return;																													// Failed
	}
	
	PVmObject right = VmPopStack(stack, True);																					// Get the right
	PVmObject left = VmPopStack(stack, True);																					// And the left
	PVmObject obj = (PVmObject)MmAllocUserMemory(sizeof(VmObject));																// Alloc space for the result
	
	if (obj == Null) {																											// Failed?
		UIntPtr old = ConGetForeground();																						// Yes, print an warning message
		
		ConSetForeground(0xFFFFFF55);
		ConWriteFormated("warning:");
		ConSetForeground(old);
		ConWriteFormated(" out of memory\r\n");
		
		if (left != &VmNullObject) {																							// Free the left and the right
			MmFreeUserMemory((UIntPtr)left);
		}
		
		if (right != &VmNullObject) {
			MmFreeUserMemory((UIntPtr)right);
		}
		
		ListAdd(stack, &VmNullObject);																							// Try to add to the stack the null object
		
		return;
	}
	
	if (left->type != right->type) {																							// Let's fix the types!
		PVmObject new = (PVmObject)MmAllocUserMemory(sizeof(VmObject));															// Alloc space for the new left
		
		if (new == Null) {																										// Out of memory?
			UIntPtr old = ConGetForeground();																					// Yes, print an warning message

			ConSetForeground(0xFFFFFF55);
			ConWriteFormated("warning:");
			ConSetForeground(old);
			ConWriteFormated(" out of memory\r\n");

			if (left != &VmNullObject) {																						// Free the left and the right
				MmFreeUserMemory((UIntPtr)left);
			}

			if (right != &VmNullObject) {
				MmFreeUserMemory((UIntPtr)right);
			}

			MmFreeUserMemory((UIntPtr)obj);																						// Free the return obj
			ListAdd(stack, &VmNullObject);																						// Try to add to the stack the null object

			return;
		}
		
		if (left->type == VM_OBJECT_TYPE_FLOAT || right->type != VM_OBJECT_TYPE_FLOAT) {
			new->type = left->type;																								// Set the new type
			new->uval = VmConvertValue(left, new->type);																		// Convert
			
			if (left != &VmNullObject) {																						// Free the old left
				MmFreeUserMemory((UIntPtr)left);
			}

			left = new;																											// Set it to the new one
		} else {
			new->type = right->type;																							// Set the new type
			new->uval = VmConvertValue(right, new->type);																		// Convert
			
			if (right != &VmNullObject) {																						// Free the old right
				MmFreeUserMemory((UIntPtr)right);
			}

			right = new;																										// Set it to the new one
		}
	}
	
	if (left->type == VM_OBJECT_TYPE_INT && right->type == VM_OBJECT_TYPE_INT) {												// Binary operation with int?
		if (type == VM_OPCODE_DIV && right->ival == 0) {																		// Yes, division by zero?
			UIntPtr old = ConGetForeground();																					// Yes, let's print an warning message
			
			ConSetForeground(0xFFFFFF55);
			ConWriteFormated("warning:");
			ConSetForeground(old);
			ConWriteFormated(" division by zero\r\n");
			
			if (left != &VmNullObject) {																						// Free the left and the right
				MmFreeUserMemory((UIntPtr)left);
			}
			
			if (right != &VmNullObject) {
				MmFreeUserMemory((UIntPtr)right);
			}
			
			MmFreeUserMemory((UIntPtr)obj);																						// Free the return obj
			ListAdd(stack, &VmNullObject);																						// Try to add to the stack the null object
			
			return;
		}
		
		switch (type) {																											// Let's do the operation!
			case VM_OPCODE_ADD: obj->type = left->type; obj->ival = left->ival + right->ival; break;
			case VM_OPCODE_SUB: obj->type = left->type; obj->ival = left->ival - right->ival; break;
			case VM_OPCODE_MUL: obj->type = left->type; obj->ival = left->ival * right->ival; break;
			case VM_OPCODE_DIV: obj->type = left->type; obj->ival = left->ival / right->ival; break;
			case VM_OPCODE_MOD: obj->type = left->type; obj->ival = left->ival % right->ival; break;
			case VM_OPCODE_SHL: obj->type = left->type; obj->ival = left->ival << right->ival; break;
			case VM_OPCODE_SHR: obj->type = left->type; obj->ival = left->ival >> right->ival; break;
			case VM_OPCODE_AND: obj->type = left->type; obj->ival = left->ival & right->ival; break;
			case VM_OPCODE_OR: obj->type = left->type; obj->ival = left->ival | right->ival; break;
			case VM_OPCODE_XOR: obj->type = left->type; obj->ival = left->ival ^ right->ival; break;
		}
	} else if (left->type == VM_OBJECT_TYPE_UINT && right->type == VM_OBJECT_TYPE_UINT) {										// Binary operation with uint?
		if (type == VM_OPCODE_DIV && right->uval == 0) {																		// Yes, division by zero?
			UIntPtr old = ConGetForeground();																					// Yes, let's print an warning message
			
			ConSetForeground(0xFFFFFF55);
			ConWriteFormated("warning:");
			ConSetForeground(old);
			ConWriteFormated(" division by zero\r\n");
			
			if (left != &VmNullObject) {																						// Free the left and the right
				MmFreeUserMemory((UIntPtr)left);
			}
			
			if (right != &VmNullObject) {
				MmFreeUserMemory((UIntPtr)right);
			}
			
			MmFreeUserMemory((UIntPtr)obj);																						// Free the return obj
			ListAdd(stack, &VmNullObject);																						// Try to add to the stack the null object
			
			return;
		}
		
		switch (type) {																											// Let's do the operation!
			case VM_OPCODE_ADD: obj->type = left->type; obj->uval = left->uval + right->uval; break;
			case VM_OPCODE_SUB: obj->type = left->type; obj->uval = left->uval - right->uval; break;
			case VM_OPCODE_MUL: obj->type = left->type; obj->uval = left->uval * right->uval; break;
			case VM_OPCODE_DIV: obj->type = left->type; obj->uval = left->uval / right->uval; break;
			case VM_OPCODE_MOD: obj->type = left->type; obj->uval = left->uval % right->uval; break;
			case VM_OPCODE_SHL: obj->type = left->type; obj->uval = left->uval << right->uval; break;
			case VM_OPCODE_SHR: obj->type = left->type; obj->uval = left->uval >> right->uval; break;
			case VM_OPCODE_AND: obj->type = left->type; obj->uval = left->uval & right->uval; break;
			case VM_OPCODE_OR: obj->type = left->type; obj->uval = left->uval | right->uval; break;
			case VM_OPCODE_XOR: obj->type = left->type; obj->uval = left->uval ^ right->uval; break;
		}
	} else {																													// Binary operation with float
		if (type == VM_OPCODE_DIV && right->fval == 0) {																		// Division by zero?
			UIntPtr old = ConGetForeground();																					// Yes, let's print an warning message
			
			ConSetForeground(0xFFFFFF55);
			ConWriteFormated("warning:");
			ConSetForeground(old);
			ConWriteFormated(" division by zero\r\n");
			
			if (left != &VmNullObject) {																						// Free the left and the right
				MmFreeUserMemory((UIntPtr)left);
			}
			
			if (right != &VmNullObject) {
				MmFreeUserMemory((UIntPtr)right);
			}
			
			MmFreeUserMemory((UIntPtr)obj);																						// Free the return obj
			ListAdd(stack, &VmNullObject);																						// Try to add to the stack the null object
			
			return;
		} else if (type >= VM_OPCODE_MOD) {																						// Invalid float operation?
			UIntPtr old = ConGetForeground();																					// Yes, let's print an warning message
			
			ConSetForeground(0xFFFFFF55);
			ConWriteFormated("warning:");
			ConSetForeground(old);
			ConWriteFormated(" invalid float operation\r\n");
			
			if (left != &VmNullObject) {																						// Free the left and the right
				MmFreeUserMemory((UIntPtr)left);
			}
			
			if (right != &VmNullObject) {
				MmFreeUserMemory((UIntPtr)right);
			}
			
			MmFreeUserMemory((UIntPtr)obj);																						// Free the return obj
			ListAdd(stack, &VmNullObject);																						// Try to add to the stack the null object
			
			return;
		}
		
		switch (type) {																											// Let's do the operation!
			case VM_OPCODE_ADD: obj->type = left->type; obj->fval = left->fval + right->fval; break;
			case VM_OPCODE_SUB: obj->type = left->type; obj->fval = left->fval - right->fval; break;
			case VM_OPCODE_MUL: obj->type = left->type; obj->fval = left->fval * right->fval; break;
			case VM_OPCODE_DIV: obj->type = left->type; obj->fval = left->fval / right->fval; break;
		}
	}
	
	if (left != &VmNullObject) {																								// Free the left and the right
		MmFreeUserMemory((UIntPtr)left);
	}
	
	if (right != &VmNullObject) {
		MmFreeUserMemory((UIntPtr)right);
	}
	
	ListAdd(stack, obj);																										// And try to add the return obj to the stack
}

PVmModule VmLoadModule(PChar filename) {
	if (filename == Null) {																										// Valid filename?
		return Null;																											// No...
	}
	
	PFsNode file = FsOpenFile(filename);																						// Let's try to open the file
	
	if (file == Null) {
		return Null;																											// Failed, probably the file doesn't exists
	}
	
	PUInt8 buf = (PUInt8)MmAllocUserMemory(file->length);																		// Alloc space for the buffer
	
	if (buf == Null) {
		FsCloseFile(file);																										// Failed
		return Null;
	}
	
	if (!FsReadFile(file, 0, file->length, buf)) {																				// Let's try to read the file!
		FsCloseFile(file);																										// Failed
		return Null;
	}
	
	FsCloseFile(file);																											// Close the file
	
	if (buf[0] != 'B' || buf[1] != 'L' || buf[2] != 'I' || buf[3] != 'S') {														// Validate the header "magic"
		MmFreeUserMemory((UIntPtr)buf);																							// Failed
		return Null;
	}
	
	PVmModule module = (PVmModule)MmAllocUserMemory(sizeof(VmModule));															// Alloc space for the VmModule struct
	
	if (module == Null) {
		MmFreeUserMemory((UIntPtr)buf);																							// Failed
		return Null;
	}
	
	module->body.head = Null;																									// Setup the instruction list (body)
	module->body.tail = Null;
	module->body.length = 0;
	module->body.free = False;
	module->body.user = True;
	
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	UInt32 blen = (buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | buf[7];														// Big endian
#else
	UInt32 blen = buf[4] | (buf[5] << 8) | (buf[6] << 16) | (buf[7] << 24);														// Little endian
#endif
	
	for (UInt32 i = 0, j = 8; i < blen; i++) {																					// Let's add the instructions!
		PVmInstruction instr = (PVmInstruction)MmAllocUserMemory(sizeof(VmInstruction));										// Alloc space for this instr
		
		if (instr == Null) {																									// Failed?
			while (module->body.length > 0) {																					// Yes, let's free everything
				MmFreeUserMemory((UIntPtr)ListRemove(&module->body, module->body.length - 1));
			}
			
			MmFreeUserMemory((UIntPtr)module);
			MmFreeUserMemory((UIntPtr)buf);
			
			return Null;
		}
		
		instr->opcode = buf[j++];																								// Set the opcode
		
		if (instr->opcode >= VM_OPCODE_LDS && instr->opcode <= VM_OPCODE_LDF) {													// Set the argument
			if (instr->opcode == VM_OPCODE_LDS) {																				// Load signed integer
				instr->arg.type = VM_OBJECT_TYPE_INT;
			} else if (instr->opcode == VM_OPCODE_LDU) {																		// Load unsigned integer
				instr->arg.type = VM_OBJECT_TYPE_UINT;
			} else {																											// Load float
				instr->arg.type = VM_OBJECT_TYPE_FLOAT;
			}
			
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
			instr->arg.uval = (buf[j] << 24) | (buf[j + 1] << 16) | (buf[j + 2] << 8) | buf[j + 3];								// Big endian
#else
			instr->arg.uval = buf[j] | (buf[j + 1] << 8) | (buf[j + 2] << 16) | (buf[j + 3] << 24);								// Little endian
#endif
			
			j += 4;
		}
		
		ListAdd(&module->body, instr);																							// (Try to) add the instruction
	}
	
	return module;
}

PVmObject VmRunModule(PVmModule module) {
	if (module == Null || ((UIntPtr)module) >= MM_USER_END) {																	// Sanity check
		return Null;																											// Failed
	}
	
	PVmObject ret = (PVmObject)MmAllocUserMemory(sizeof(VmObject));																// Alloc space for the ret object
	
	if (ret == Null) {
		return Null;																											// Failed
	}
	
	ret->type = VM_OBJECT_TYPE_INT;																								// Set the default type
	ret->ival = 0;																												// And value
	
	PList stack = ListNew(True, True);																							// Try to alloc the stack
	
	if (stack == Null) {
		MmFreeUserMemory((UIntPtr)ret);																							// Failed...
		return Null;
	}
	
	PList cstack = ListNew(True, True);																							// Try to alloc the call stack
	
	if (cstack == Null) {
		ListFree(stack);																										// Failed...
		MmFreeUserMemory((UIntPtr)ret);
		return Null;
	}
	
	for (UIntPtr i = 0; i < module->body.length; ) {																			// Time to interpret!
		PVmInstruction instr = (PVmInstruction)ListGet(&module->body, i++);
		
		switch (instr->opcode) {
		case VM_OPCODE_ADD:
		case VM_OPCODE_SUB:
		case VM_OPCODE_MUL:
		case VM_OPCODE_DIV:
		case VM_OPCODE_MOD:
		case VM_OPCODE_SHL:
		case VM_OPCODE_SHR:
		case VM_OPCODE_AND:
		case VM_OPCODE_OR:
		case VM_OPCODE_XOR: {																									// Binary operations
			VmBinaryOperation(stack, instr->opcode);
			break;
		}
		case VM_OPCODE_LDS:
		case VM_OPCODE_LDU:
		case VM_OPCODE_LDF: {																									// Load operations
			PVmObject obj = (PVmObject)MmAllocUserMemory(sizeof(VmObject));														// Alloc space for the loaded object
			
			if (obj == Null) {
				UIntPtr old = ConGetForeground();																				// Failed, let's show an warning
				
				ConSetForeground(0xFFFFFF55);
				ConWriteFormated("warning:");
				ConSetForeground(old);
				ConWriteFormated(" out of memory\r\n");
			} else {
				obj->type = instr->arg.type;																					// Set the type
				obj->uval = instr->arg.uval;																					// Set the value
				ListAdd(stack, obj);																							// And (try to) add to the stack
			}
			
			break;
		}
		case VM_OPCODE_POP: {																									// Pop
			PVmObject obj = VmPopStack(stack, True);																			// Pop the last entry from the stack
			
			if (obj != &VmNullObject) {																							// And free it!
				MmFreeUserMemory((UIntPtr)obj);
			}
			
			break;
		}
		case VM_OPCODE_BR: {																									// Jump
			i = instr->arg.uval + 1;
			break;
		}
		case VM_OPCODE_CALL: {																									// Call
			PVmObject obj = (PVmObject)MmAllocUserMemory(sizeof(VmObject));														// Alloc space for the call stack save object
			
			if (obj == Null) {
				UIntPtr old = ConGetForeground();																				// Failed, let's show an warning
				
				ConSetForeground(0xFFFFFF55);
				ConWriteFormated("warning:");
				ConSetForeground(old);
				ConWriteFormated(" out of memory\r\n");
			} else {
				obj->type = VM_OBJECT_TYPE_UINT;																				// Type is uint
				obj->uval = i + 1;																								// Value is the current ip + 1
				ListAdd(cstack, obj);
			}
			
			i = instr->arg.uval + 1;																							// Jump!
			
			break;
		}
		case VM_OPCODE_RET: {																									// Return from method
			PVmObject obj = VmPopCallStack(cstack);																				// Pop the last entry from the call stack
			
			if (obj->type != VM_OBJECT_TYPE_UINT) {																				// Convert to uint (if we need to do it)
				i = VmConvertValue(obj, VM_OBJECT_TYPE_UINT) + 1;
			} else {
				i = obj->uval + 1;
			}
			
			if (obj != &VmNullObject) {																							// Free it
				MmFreeUserMemory((UIntPtr)obj);
			}
			
			break;
		}
		default: {
			UIntPtr old = ConGetForeground();																					// Unimplemented instruction, let's show an warning
			
			ConSetForeground(0xFFFFFF55);
			ConWriteFormated("warning:");
			ConSetForeground(old);
			ConWriteFormated(" unimplemented instruction %d\r\n", instr->opcode);
			
			break;
		}
		}
	}
	
	if (stack->length > 0) {																									// Set the return value?
		PVmObject obj = VmPopStack(stack, True);																				// Yes, pop
		
		ret->type = obj->type;																									// Set the type
		ret->uval = obj->uval;																									// Set the value
		
		if (obj != &VmNullObject) {																								// Free
			MmFreeUserMemory((UIntPtr)obj);
		}
	}
	
	ListFree(cstack);																											// Free the call stack
	ListFree(stack);																											// Free the stack
	
	return ret;
}
