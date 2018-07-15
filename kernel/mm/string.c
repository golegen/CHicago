// File author is Ítalo Lima Marconato Matias
//
// Created on July 15 of 2018, at 19:05 BRT
// Last edited on July 15 of 2018, at 20:05 BRT

#include <chicago/alloc.h>

PVoid StrCopyMemory(PVoid dest, PVoid src, UIntPtr count) {
	if ((dest == Null) || (src == Null) || (count == 0) || (src == dest)) {			// Destination is an Null pointer? Source is an Null pointer? Zero-sized copy? Destination is Source?
		return dest;																// Yes
	}
	
	UIntPtr n = (count + 7) / 8;													// For memory copy we can use the Duff's Device (Loop unroll)!
	PUInt8 mp1 = dest;
	PUInt8 mp2 = src;
	
	switch (count % 8) {
		case 0: do { *mp1++ = *mp2++;
		case 7:      *mp1++ = *mp2++;
		case 6:      *mp1++ = *mp2++;
		case 5:      *mp1++ = *mp2++;
		case 4:      *mp1++ = *mp2++;
		case 3:      *mp1++ = *mp2++;
		case 2:      *mp1++ = *mp2++;
		case 1:      *mp1++ = *mp2++;
		        } while (--n > 0);
	}
	
	return dest;
}

PVoid StrSetMemory(PVoid dest, UInt8 val, UIntPtr count) {
	if ((dest == Null) || (count == 0)) {											// Destination is an Null pointer? Zero-sized set?
		return dest;																// Yes
	}
	
	UIntPtr n = (count + 7) / 8;													// In this function we're also going to use Duff's Device
	PUInt8 mp1 = dest;
	
	switch (count % 8) {
		case 0: do { *mp1++ = val;
		case 7:      *mp1++ = val;
		case 6:      *mp1++ = val;
		case 5:      *mp1++ = val;
		case 4:      *mp1++ = val;
		case 3:      *mp1++ = val;
		case 2:      *mp1++ = val;
		case 1:      *mp1++ = val;
		        } while (--n > 0);
	}
	
	return dest;
}

Boolean StrCompareMemory(PVoid m1, PVoid m2, UIntPtr count) {
	if ((m1 == Null) || (m2 == Null) || (count == 0)) {								// m1 is an Null pointer? m2 is an Null pointer? Zero-sized compare?
		return False;																// Yes
	}
	
	UIntPtr n = (count + 7) / 8;													// Somehow, we're going to use Duff's Device (i have to stop using it for everything)
	PUInt8 mp1 = m1;
	PUInt8 mp2 = m2;
	
	switch (count % 8) {
		case 0: do { if (*mp1++ != *mp2++) return False;
		case 7:      if (*mp1++ != *mp2++) return False;
		case 6:      if (*mp1++ != *mp2++) return False;
		case 5:      if (*mp1++ != *mp2++) return False;
		case 4:      if (*mp1++ != *mp2++) return False;
		case 3:      if (*mp1++ != *mp2++) return False;
		case 2:      if (*mp1++ != *mp2++) return False;
		case 1:      if (*mp1++ != *mp2++) return False;
		        } while (--n > 0);
	}
	
	return True;
}

UIntPtr StrGetLength(PChar str) {
	if (str == Null) {																// Str is an Null pointer?
		return 0;																	// Yes
	}
	
	UIntPtr n = 0;																	// This time, we're not going to use Duff's Device, WE'RE GOING TO USE... loop unroll... it's almost the samething so...
	
	while (1) {
		if (*(str + n++) == 0) { return n - 1; }
		if (*(str + n++) == 0) { return n - 1; }
		if (*(str + n++) == 0) { return n - 1; }
		if (*(str + n++) == 0) { return n - 1; }
		if (*(str + n++) == 0) { return n - 1; }
		if (*(str + n++) == 0) { return n - 1; }
		if (*(str + n++) == 0) { return n - 1; }
		if (*(str + n++) == 0) { return n - 1; }
	}
}

Boolean StrCompare(PChar dest, PChar src) {
	return StrCompareMemory(dest, src, StrGetLength(dest));							// Just redirect to the StrCompareMemory function
}

PChar StrCopy(PChar dest, PChar src) {
	return StrCopyMemory(dest, src, StrGetLength(dest) + 1);						// Just redirect to StrCopyMemory function
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

PChar StrTokenize(PChar str, PChar delim) {
	if (delim == Null) {															// The delimiter list is an Null pointer?
		return Null;																// ...
	}
	
	static PChar buf = Null;
	
	if (str != Null) {																// First call?
		buf = str;																	// Yes, set the buffer
	}
	
	if (buf[0] == '\0') {															// End of the buffer (everthing is tokenized)?
		return Null;																// Yes, so we don't have anything!
	}
	
	PChar ret = buf;
	
	for (PChar b = buf; *b != '\0'; b++) {
		for (PChar d = delim; *d != '\0'; d++) {
			if (*b == *d) {
				*b = '\0';
				buf = b + 1;
				
				if (b == ret) {														// We may find more then one delim entries in the str, ie: str is " -Hello, World!" and delim is " -,!"
					ret++;
					continue;
				}
				
				return ret;
			}
		}
	}
	
	return ret;
}

PChar StrDuplicate(PChar str) {
	PChar ret = (PChar)MemAllocate(StrGetLength(str) + 1);
	
	if (ret == Null) {
		return Null;
	}
	
	return StrCopy(ret, str);
}
