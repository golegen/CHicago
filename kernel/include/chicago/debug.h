// File author is Ítalo Lima Marconato Matias
//
// Created on May 11 of 2018, at 23:24 BRT
// Last edited on June 30 of 2018, at 17:08 BRT

#ifndef __CHICAGO_DEBUG_H__
#define __CHICAGO_DEBUG_H__

Void DbgWriteCharacter(Char data);
Void DbgWriteString(Const PChar data);
Void DbgWriteInteger(UIntPtr data, UInt8 base);
Void DbgWriteFormated(Const PChar data, ...);

#endif		// __CHICAGO_DEBUG_H__
