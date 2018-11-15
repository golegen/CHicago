// File author is Ítalo Lima Marconato Matias
//
// Created on October 25 of 2018, at 14:23 BRT
// Last edited on November 15 of 2018, at 15:59 BRT

#include <chicago/alloc.h>
#include <chicago/arch.h>
#include <chicago/config.h>
#include <chicago/console.h>
#include <chicago/file.h>
#include <chicago/string.h>

PList ConfFields = Null;
Char ConfStringEnd = 0;

static Boolean ConfIsWhiteSpace(Char c) {
	return c == ' ' || c == '\r' || c == '\t';														// Whitespace: ' ', '\r' (carriage return), '\t' (tab)
}

static Boolean ConfIsIdentifier(Char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||										// Identifier: 'a' to 'z', 'A' to 'Z', '\', '-', '_', '@' and '.'
			c == '\\' || c == '-' || c == '_' || c == '@' ||
			c == '.';
}

static Boolean ConfIsNumber(Char c) {
	return c >= '0' && c <= '9';																	// Number: 0 to 9
}

static Boolean ConfIsIdentifierOrNumber(Char c) {
	return ConfIsIdentifier(c) || ConfIsNumber(c);													// Identifier or Number
}

static Boolean ConfCheckStringEnd(Char c) {
	return c != ConfStringEnd;																		// String: ' or "
}

static UIntPtr ConfGetLength(PChar buffer, UIntPtr pos, UIntPtr len, Boolean (*check)(Char)) {
	UIntPtr i = 0;
	
	while (((pos + i) < len) && check(buffer[pos + i])) {											// Use the check function!
		i++;
	}
	
	return i;
}

static PChar ConfStringFromChar(Char c) {
	PChar str = (PChar)MemAllocate(2);																// 1 char for the char and 1 for the terminator
	
	if (str != Null) {
		str[0] = c;																					// Copy the character to the string
		str[1] = 0;
	}
	
	return str;
}

static Boolean ConfAcceptToken(PList toks, UIntPtr pos, PChar value) {
	if (toks == Null || value == Null) {															// Checks
		return False;																				// Failed
	} else if (pos >= toks->length) {
		return False;																				// Failed
	}
	
	PChar str = (PChar)ListGet(toks, pos);
	
	if (StrGetLength(str) != StrGetLength(value)) {													// Same length?
		return False;																				// Nope
	}
	
	return StrCompare(str, value);																	// Compare!
}

static Void ConfExpectToken(PList toks, UIntPtr pos, PChar value, Boolean enums, Boolean equals) {
	if (toks == Null || value == Null) {															// Checks
		ConWriteFormated("PANIC! Couldn't parse the configuration file\r\n");						// Failed
		ArchHalt();																					// Halt
	} else if (pos >= toks->length) {
		ConWriteFormated("PANIC! Couldn't parse the configuration file\r\n");						// Failed
		ArchHalt();																					// Halt
	}
	
	PChar str = (PChar)ListGet(toks, pos);
	
	if (!enums && ConfIsNumber(str[0])) {															// Accept numbers?
		ConWriteFormated("PANIC! Couldn't parse the configuration file\r\n");						// Nope
		ArchHalt();																					// Halt
	}
	
	if ((StrGetLength(str) != StrGetLength(value)) && equals) {										// Same length?
		ConWriteFormated("PANIC! Couldn't parse the configuration file\r\n");						// Nope
		ArchHalt();																					// Halt
	}
	
	Boolean scmp = StrCompare(str, value);
	
	if (!scmp && equals) {																			// Equals?
		ConWriteFormated("PANIC! Couldn't parse the configuration file\r\n");						// Nope
		ArchHalt();																					// Halt
	} else if (scmp && !equals) {
		ConWriteFormated("PANIC! Couldn't parse the configuration file\r\n");						// Yes, but the user don't want it to be equal...
		ArchHalt();																					// Halt
	}
}

static PList ConfLexBuffer(PChar buffer, UIntPtr len) {
	if (buffer == Null) {																			// Valid buffer?
		return Null;																				// Nope
	}
	
	PList toks = ListNew(True);																		// Alloc the token list
	
	if (toks == Null) {
		return Null;																				// Failed
	}
	
	UIntPtr pos = 0;
	
	while (pos < len) {
		while (ConfIsWhiteSpace(buffer[pos])) {														// Skip the whitespaces
			pos++;
		}
		
		if (buffer[pos] == ';') {																	// Comment?
			pos++;																					// Yes
			
			while (pos < len && buffer[pos] != '\n') {												// Consume until the next line (or the end of file)
				pos++;
			}
		} else if (buffer[pos] == '\n' || buffer[pos] == '=' || buffer[pos] == ',') {				// New line, equal sign or comma?
			PChar str = ConfStringFromChar(buffer[pos]);											// Yes, try to create the string from the character
			
			if (str == Null) {
				ListFree(toks);																		// Failed...
				return Null;
			}
			
			if (!ListAdd(toks, str)) {																// Add this token
				ListFree(toks);																		// Failed
				return Null;
			}
		} else if (buffer[pos] == '"' || buffer[pos] == '\'') {										// String?
			ConfStringEnd = buffer[pos++];															// Yes, set the string end character
			
			UIntPtr length = ConfGetLength(buffer, pos, len, ConfCheckStringEnd);					// Get the length
			
			if (pos + length >= len) {
				ListFree(toks);																		// Unterminated string...
				return Null;
			}
			
			PChar str = (PChar)MemAllocate(length + 1);												// Allocate memory
			
			if (str == Null) {
				ListFree(toks);																		// Failed...
				return Null;
			}
			
			for (UIntPtr i = 0; i < length; i++) {													// Copy the string
				str[i] = buffer[pos++];
			}
			
			str[length] = 0;																		// End the string
			
			if (!ListAdd(toks, str)) {																// Add this token
				ListFree(toks);																		// Failed
				return Null;
			}
		} else if (ConfIsIdentifier(buffer[pos])) {													// Identifier?
			UIntPtr length = ConfGetLength(buffer, pos, len, ConfIsIdentifierOrNumber);				// Yes, get the length
			PChar str = (PChar)MemAllocate(length + 1);												// Allocate memory
			
			if (str == Null) {
				ListFree(toks);																		// Failed...
				return Null;
			}
			
			for (UIntPtr i = 0; i < length; i++) {													// Copy the string
				str[i] = buffer[pos++];
			}
			
			str[length] = 0;																		// End the string
			pos--;																					// Go one character back
			
			if (!ListAdd(toks, str)) {																// Add this token
				ListFree(toks);																		// Failed
				return Null;
			}
		} else if (ConfIsNumber(buffer[pos])) {														// Number?
			UIntPtr length = ConfGetLength(buffer, pos, len, ConfIsNumber);							// Yes, get the length
			PChar str = (PChar)MemAllocate(length + 1);												// Allocate memory
			
			if (str == Null) {
				ListFree(toks);																		// Failed...
				return Null;
			}
			
			for (UIntPtr i = 0; i < length; i++) {													// Copy the string
				str[i] = buffer[pos++];
			}
			
			str[length] = 0;																		// End the string
			pos--;																					// Go one character back
			
			if (!ListAdd(toks, str)) {																// Add this token
				ListFree(toks);																		// Failed
				return Null;
			}
		} else {
			ListFree(toks);																			// Invalid character...
			return Null;
		}
		
		pos++;
	}
	
	toks->free = False;
	
	return toks;
}

static Void ConfParseTokens(PList toks) {
	if (toks == Null) {																				// Valid tok buffer?
		ConWriteFormated("PANIC! Couldn't parse the configuration file\r\n");						// No
		ArchHalt();																					// Halt
	}
	
	UIntPtr pos = 0;
	
	while (pos < toks->length) {																	// Let's parse!
		while (ConfAcceptToken(toks, pos, "\n")) {													// Consume new lines
			pos++;
		}
		
		if (pos >= toks->length) {																	// Break if we don't have any token to consume
			break;
		}
		
		ConfExpectToken(toks, pos, "=", False, False);												// We expect to find an identifier here
		ConfExpectToken(toks, pos, ",", False, False);
		ConfExpectToken(toks, pos, "\n", False, False);
		
		PConfField field = (PConfField)MemAllocate(sizeof(ConfField));								// Alloc space for the field
		
		if (field == Null) {
			ConWriteFormated("PANIC! Couldn't parse the configuration file\r\n");					// Failed
			ArchHalt();																				// Halt
		}
		
		field->name = (PChar)ListGet(toks, pos++);													// Set the name
		
		if (ConfAcceptToken(toks, pos, "=")) {														// Set the value?
			pos++;																					// Yes
			
			ConfExpectToken(toks, pos, "=", True, False);											// Accept almost anything here
			ConfExpectToken(toks, pos, ",", True, False);
			ConfExpectToken(toks, pos, "\n", True, False);
			
			field->value = (PChar)ListGet(toks, pos++);												// Set the value
		} else {
			field->value = StrDuplicate("1");														// Just set the default value
		}
		
		if (!ConfAcceptToken(toks, pos, "\n")) {													// Set the attributes?
			field->attrs = ListNew(True);															// Yes, init the list
			
			if (field->attrs == Null) {
				ConWriteFormated("PANIC! Couldn't parse the configuration file\r\n");				// Failed
				ArchHalt();																			// Halt
			}
			
			ConfExpectToken(toks, pos++, ",", False, True);											// Now we should have an comma here
start:		ConfExpectToken(toks, pos, "=", True, False);											// Accept almost anything here
			ConfExpectToken(toks, pos, ",", True, False);
			ConfExpectToken(toks, pos, "\n", True, False);
			
			if (!ListAdd(field->attrs, (PChar)ListGet(toks, pos++))) {								// And try to add this attribute
				ConWriteFormated("PANIC! Couldn't parse the configuration file\r\n");				// Failed
				ArchHalt();																			// Halt
			}
			
			if (ConfAcceptToken(toks, pos, ",")) {													// More attributes?
				pos++;																				// Yes!
				goto start;
			}
		} else {
			field->attrs = Null;																	// Set the attr list to Null
		}
		
		ConfExpectToken(toks, pos, "\n", False, True);												// Expect new line
		
		if (field->value == Null) {
			ConWriteFormated("PANIC! Couldn't parse the configuration file\r\n");					// No valid value...
			ArchHalt();																				// Halt
		}
		
		if (!ListAdd(ConfFields, field)) {															// Try to add this field
			ConWriteFormated("PANIC! Couldn't parse the configuration file\r\n");					// Failed
			ArchHalt();																				// Halt
		}
	}
}

Void ConfIterate(Void (*it)(PConfField)) {
	ListForeach(ConfFields, i) {																	// Iterate the list!
		it((PConfField)i->data);
	}
}

Void ConfInit(Void) {
	ConfFields = ListNew(False);																	// Init the config fields list
	
	if (ConfFields == Null) {
		ConWriteFormated("PANIC! Couldn't open the configuration file\r\n");						// Failed...
		ArchHalt();																					// Halt
	}
	
	PFsNode file = FsOpenFile("\\Boot\\bootmgr.conf");												// Try to open the config file
	
	if (file == Null) {
		ConWriteFormated("PANIC! Couldn't open the configuration file\r\n");						// Failed...
		ArchHalt();																					// Halt
	}
	
	PChar buffer = (PChar)MemAllocate(file->length);												// Alloc space for reading the file
	
	if (buffer == Null) {
		ConWriteFormated("PANIC! Couldn't read the configuration file\r\n");						// Failed...
		ArchHalt();																					// Halt
	}
	
	if (!FsReadFile(file, 0, file->length, (PUInt8)buffer)) {										// Read the file!
		ConWriteFormated("PANIC! Couldn't read the configuration file\r\n");						// Failed...
		ArchHalt();																					// Halt
	}
	
	FsCloseFile(file);																				// Close the file
	
	PList toks = ConfLexBuffer(buffer, file->length);												// Lex
	
	if (toks == Null) {
		ConWriteFormated("PANIC! Couldn't lex the configuration file\r\n");							// Failed...
		ArchHalt();																					// Halt
	}
	
	MemFree((UIntPtr)buffer);																		// Free the buffer
	ConfParseTokens(toks);																			// Parse the token list
	ListFree(toks);																					// Free the token list
}
