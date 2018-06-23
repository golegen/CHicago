// File author is Ítalo Lima Marconato Matias
//
// Created on May 11 of 2018, at 23:50 BRT
// Last edited on May 27 of 2018, at 13:54 BRT

#include <chicago/arch/port.h>
#include <chicago/arch/serial.h>

Void DebugWriteCharacter(Char data) {
	while ((PortInByte(COM1_PORT + 5) & 0x20) == 0) ;
	PortOutByte(COM1_PORT, data);
}

Void DebugWriteString(Const PChar data) {
	for (UInt32 i = 0; data[i] != '\0'; i++) {
		DebugWriteCharacter(data[i]);
	}
}

Void DebugWriteInteger(UInt32 data, UInt8 base) {
	if (data == 0) {
		DebugWriteCharacter('0');
		return;
	}
	
	static Char buf[32] = { 0 };
	Int i = 30;
	
	for (; data && i; i--, data /= base) {
		buf[i] = "0123456789ABCDEF"[data % base];
	}
	
	DebugWriteString(&buf[i + 1]);
}

Void DebugWriteFormated(Const PChar data, ...) {
	VariadicList va;
	VariadicStart(va, data);					// Let's start our va list with the arguments provided by the user (if any)
	
	for (UInt32 i = 0; data[i] != '\0'; i++) {
		if (data[i] != '%') {					// It's an % (integer, string, character or other)?
			DebugWriteCharacter(data[i]);		// Nope
		} else {
			switch (data[++i]) {				// Yes! So let's parse it!
			case 's': {							// String
				DebugWriteString((PChar)VariadicArg(va, PChar));
				break;
			}
			case 'c': {							// Character
				DebugWriteCharacter((Char)VariadicArg(va, Int));
				break;
			}
			case 'd': {							// Decimal Number
				DebugWriteInteger((UInt32)VariadicArg(va, UInt32), 10);
				break;
			}
			case 'x': {							// Hexadecimal Number
				DebugWriteInteger((UInt32)VariadicArg(va, UInt32), 16);
				break;
			}
			default: {							// None of the others...
				DebugWriteCharacter(data[i]);
				break;
			}
			}
		}
	}
	
	VariadicEnd(va);
}

Void SerialInit(UInt16 port) {
	PortOutByte(port + 1, 0x00);		// Disable all interrupts
	PortOutByte(port + 3, 0x80);		// Enable DLAB (set baud rate divisor)
	PortOutByte(port + 0, 0x03);		// Set divisor to 3 (lo byte) 38400
										// baud (hi byte)
	PortOutByte(port + 1, 0x00);
	PortOutByte(port + 3, 0x03);		// 8 bits, no parity, one stop bit
	PortOutByte(port + 2, 0xC7);		// Enable FIFO, clear then with
										// 14-byte threshold
	PortOutByte(port + 4, 0x0B);		// IRQs enables, RTS/DSR set
}
