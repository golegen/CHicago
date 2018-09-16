// File author is Ítalo Lima Marconato Matias
//
// Created on September 16 of 2018, at 12:33 BRT
// Last edited on September 16 of 2018, at 12:36 BRT

#include <chicago/arch/idt.h>

#include <chicago/sc.h>

Void ArchScHandler(PRegisters regs) {
	switch (regs->eax) {
	case 0x00: {																										// Boolean SysGetVersion(PSystemVersion ver)
		regs->eax = ScSysGetVersion((PSystemVersion)regs->eax);
		break;
	}
	case 0x01: {																										// UIntPtr VirtAllocAddress(UIntPtr addr, UIntPtr size, UInt32 flags)
		regs->eax = ScVirtAllocAddress(regs->eax, regs->ebx, regs->ecx);
		break;
	}
	case 0x02: {																										// Boolean VirtFreeAddress(UIntPtr addr, UIntPtr size)
		regs->eax = ScVirtFreeAddress(regs->eax, regs->ebx);
		break;
	}
	case 0x03: {																										// UInt32 VirtQueryProtection(UIntPtr addr)
		regs->eax = ScVirtQueryProtection(regs->eax);
		break;
	}
	case 0x04: {																										// Boolean VirtChangeProtection(UIntPtr addr, UIntPtr size, UInt32 flags)
		regs->eax = ScVirtChangeProtection(regs->eax, regs->ebx, regs->ecx);
		break;
	}
	case 0x05: {																										// UIntPtr PsGetTID(Void)
		regs->eax = ScPsGetTID();
		break;
	}
	case 0x06: {																										// UIntPtr PsGetPID(Void)
		regs->eax = ScPsGetPID();
		break;
	}
	case 0x07: {																										// PThreadData PsGetThreadData(Void)
		regs->eax = (UIntPtr)ScPsGetThreadData();
		break;
	}
	case 0x08: {																										// Void PsSleep(UIntPtr ms)
		ScPsSleep(regs->eax);
		break;
	}
	case 0x09: {																										// Boolean PsWaitThread(UIntPtr tid)
		regs->eax = ScPsWaitThread(regs->eax);
		break;
	}
	case 0x0A: {																										// Boolean PsWaitProcess(UIntPtr tid)
		regs->eax = ScPsWaitProcess(regs->eax);
		break;
	}
	case 0x0B: {																										// Boolean PsLock(PLock lock)
		regs->eax = ScPsLock((PLock)regs->eax);
		break;
	}
	case 0x0C: {																										// Boolean PsUnlock(PLock lock)
		regs->eax = ScPsUnlock((PLock)regs->eax);
		break;
	}
	case 0x0D: {																										// Void PsExitProcess(UIntPtr val)
		ScPsExitProcess(regs->eax);
		break;
	}
	case 0x0E: {																										// Void PsExitThread(UIntPtr val)
		ScPsExitThread(regs->eax);
		break;
	}
	case 0x0F: {																										// Void PsForceSwitch(Void)
		ScPsForceSwitch();
		break;
	}
	case 0x10: {																										// Int FsOpenFile(PChar path)
		regs->eax = ScFsOpenFile((PChar)regs->eax);
		break;
	}
	case 0x11: {																										// Boolean FsCloseFile(Int file)
		regs->eax = ScFsCloseFile(regs->eax);
		break;
	}
	case 0x12: {																										// Boolean FsReadFile(Int file, UIntPtr off, UIntPtr len, PUInt8 buf)
		regs->eax = ScFsReadFile(regs->eax, regs->ebx, regs->ecx, (PUInt8)regs->edx);
		break;
	}
	case 0x13: {																										// Boolean FsWriteFile(Int file, UIntPtr off, UIntPtr len, PUInt8 buf)
		regs->eax = ScFsWriteFile(regs->eax, regs->ebx, regs->ecx, (PUInt8)regs->edx);
		break;
	}
	case 0x14: {																										// Boolean FsMountFile(PChar path, PChar file, PChar type)
		regs->eax = ScFsMountFile((PChar)regs->eax, (PChar)regs->ebx, (PChar)regs->ecx);
		break;
	}
	case 0x15: {																										// Boolean FsUmountFile(PChar path)
		regs->eax = ScFsUmountFile((PChar)regs->eax);
		break;
	}
	case 0x16: {																										// Boolean FsReadDirectoryEntry(Int dir, UIntPtr entry, PChar out)
		regs->eax = ScFsReadDirectoryEntry(regs->eax, regs->ebx, (PChar)regs->ecx);
		break;
	}
	case 0x17: {																										// Int FsFindInDirectory(Int dir, PChar name)
		regs->eax = ScFsFindInDirectory(regs->eax, (PChar)regs->ebx);
		break;
	}
	case 0x18: {																										// Boolean FsControlFile(Int file, UIntPtr cmd, PUInt8 ibuf, PUInt8 obuf)
		regs->eax = ScFsControlFile(regs->eax, regs->ebx, (PUInt8)regs->ecx, (PUInt8)regs->edx);
		break;
	}
	case 0x19: {																										// UIntPtr FsGetFileSize(Int file)
		regs->eax = ScFsGetFileSize(regs->eax);
		break;
	}
	default: {
		regs->eax = (UIntPtr)-1;
		break;
	}
	}
}

Void ArchInitSc(Void) {
	IDTRegisterInterruptHandler(0x3F, ArchScHandler);
}
