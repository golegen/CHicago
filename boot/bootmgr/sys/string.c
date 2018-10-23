// File author is Ítalo Lima Marconato Matias
//
// Created on July 15 of 2018, at 19:05 BRT
// Last edited on October 23 of 2018, at 15:26 BRT

#include <chicago/types.h>

PVoid StrCopyMemory(PVoid dest, PVoid src, UIntPtr count) {
	if ((dest == Null) || (src == Null) || (count == 0) || (src == dest)) {			// Destination is an Null pointer? Source is an Null pointer? Zero-sized copy? Destination is Source?
		return dest;																// Yes
	}
	
	for (UIntPtr i = 0; i < count; i++) {											// GCC should optimize this for us :)
		((PUInt8)dest)[i] = ((PUInt8)src)[i];
	}
	
	return dest;
}

PVoid StrCopyMemory16(PVoid dest, PVoid src, UIntPtr count) {
	if ((dest == Null) || (src == Null) || (count == 0) || (src == dest)) {			// Destination is an Null pointer? Source is an Null pointer? Zero-sized copy? Destination is Source?
		return dest;																// Yes
	}
	
	for (UIntPtr i = 0; i < count; i++) {											// GCC should optimize this for us :)
		((PUInt16)dest)[i] = ((PUInt16)src)[i];
	}
	
	return dest;
}

PVoid StrSetMemory(PVoid dest, UInt8 val, UIntPtr count) {
	if ((dest == Null) || (count == 0)) {											// Destination is an Null pointer? Zero-sized set?
		return dest;																// Yes
	}
	
	for (UIntPtr i = 0; i < count; i++) {											// GCC should optimize this for us :)
		((PUInt8)dest)[i] = val;
	}
	
	return dest;
}

PVoid StrSetMemory16(PVoid dest, UInt16 val, UIntPtr count) {
	if ((dest == Null) || (count == 0)) {											// Destination is an Null pointer? Zero-sized set?
		return dest;																// Yes
	}
	
	for (UIntPtr i = 0; i < count; i++) {											// GCC should optimize this for us :)
		((PUInt16)dest)[i] = val;
	}
	
	return dest;
}

Boolean StrCompareMemory(PVoid m1, PVoid m2, UIntPtr count) {
	if ((m1 == Null) || (m2 == Null) || (count == 0)) {								// m1 is an Null pointer? m2 is an Null pointer? Zero-sized compare?
		return False;																// Yes
	}
	
	PUInt8 mp1 = m1;
	PUInt8 mp2 = m2;
	
	for (UIntPtr i = 0; i < count; i++) {											// GCC should optimize this for us :)
		if (*mp1++ != *mp2++) {
			return False;
		}
	}
	
	return True;
}

UIntPtr StrGetLength(PChar str) {
	if (str == Null) {																// Str is an Null pointer?
		return 0;																	// Yes
	}
	
	UIntPtr n = 0;
	
	for (; str[n] != 0; n++) ;														// Again, GCC should optimize this for us (if possible)
	
	return n;
}

Boolean StrCompare(PChar dest, PChar src) {
	return StrCompareMemory(dest, src, StrGetLength(dest));							// Just redirect to the StrCompareMemory function
}

PChar StrCopy(PChar dest, PChar src) {
	return StrCopyMemory(dest, src, StrGetLength(src) + 1);							// Just redirect to StrCopyMemory function
}

Void StrConcatenate(PChar dest, PChar src) {
	if ((dest == Null) || (src == Null)) {											// Destination is an Null pointer? Source is an Null pointer?
		return;																		// Yes :(
	}
	
	PChar end = dest + StrGetLength(dest);											// Let's do it!
	
	StrCopyMemory(end, src, StrGetLength(src));										// Let's append the src to dest
	
	end += StrGetLength(src);														// And put an 0 (NUL) at the end
	*end = '\0';
}
