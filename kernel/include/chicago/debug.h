// File author is Ítalo Lima Marconato Matias
//
// Created on May 11 of 2018, at 23:24 BRT
// Last edited on June 29 of 2018, at 17:53 BRT

#ifndef __CHICAGO_DEBUG_H__
#define __CHICAGO_DEBUG_H__

Void DebugWriteCharacter(Char data);
Void DebugWriteString(Const PChar data);
Void DebugWriteInteger(UIntPtr data, UInt8 base);
Void DebugWriteFormated(Const PChar data, ...);

#endif		// __CHICAGO_DEBUG_H__
