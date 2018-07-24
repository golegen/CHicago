// File author is Ítalo Lima Marconato Matias
//
// Created on July 15 of 2018, at 19:05 BRT
// Last edited on July 24 of 2018, at 14:10 BRT

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
	
	UIntPtr n = 0;																	// This time, we're not going to use Duff's Device, WE'RE GOING TO USE... loop unroll... it's almost the same thing so...
	
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

PChar StrDuplicate(PChar str) {
	PChar ret = (PChar)MemAllocate(StrGetLength(str) + 1);
	
	if (ret == Null) {
		return Null;
	}
	
	return StrCopy(ret, str);;
}

PChar StrTokenize(PChar str, PChar delim) {
	static PChar temp = Null;
	
	if (str != Null) {																// First call?
		if (temp != Null) {															// Free the current temp?
			MemFree((UIntPtr)temp);													// Yup
		}
		
		temp = StrDuplicate(str);													// Yes, try to set the copy the str to temp
		
		if (temp == Null) {															// Failed?
			return Null;															// Yes
		}
	} else if (temp == Null) {														// Not the first call but temp is Null?
		return Null;																// Yes, so return Null
	} else {
		str = temp;
	}
	
	UIntPtr chars = 0;
	UIntPtr flag = 0;
	
	while (*temp) {
		for (PChar d = delim; *d != '\0'; d++) {
			if (*temp == *d) {														// Found delim in the string?
				if (chars == 0) {													// First character in the string?
					flag = 1;														// Yes, there may be other after it, so go to the next one
					str++;
				} else {
					temp++;															// No, so we can return
					str[chars] = '\0';
					
					return str;
				}
			}
		}
		
		if (flag == 0) {															// Found delim?
			chars++;																// No, so increase chars
		}
		
		temp++;
		flag = 0;
	}
	
	temp = Null;
	str[chars] = '\0';
	str = StrDuplicate(str);														// *HACKHACKHACK*
	
	MemFree((UIntPtr)temp);
	
	return str;
}
