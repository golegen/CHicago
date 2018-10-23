; File author is Ítalo Lima Marconato Matias
;
; Created on October 22 of 2018, at 13:36 BRT
; Last edited on October 23 of 2018, at 14:51 BRT

bits 16

global BootEntry
BootEntry:
	cli
	
	xor ax, ax							; Setup segments
	mov ds, ax
	mov es, ax
	mov ax, 0x9000
	mov ss, ax
	mov sp, 0xFFFF
	
	sti
	
	mov byte [BootDrive], dl			; Save dl
.FindRootDirectory:
	mov bx, 0x10						; Start with sector 0x10
	mov cx, 1							; Just one sector at time
	mov di, 0x500						; Read into 0x500
.ReadVolume:
	call ReadSectors					; Read the sector
	
	mov al, [es:di]						; Get type
	cmp al, 0x01						; Found?
	je .Found							; Yes!
	cmp al, 0xFF						; Terminator?
	je .NotFound						; Yes...
	
	inc bx								; Move on to next sector
	jmp .ReadVolume
.Found:
	mov bx, [es:di + 156 + 2]			; Get root directory sector
.ProcessSector0:
	mov cx, 1							; Just one sector at time
	mov di, 0x500						; Read into 0x500
	call ReadSectors					; Load the current dir sector into buffer
.ProcessDirEntry0:
	xor ax, ax							; Terminator?
	mov al, [es:di]
	cmp al, 0
	je .FolderNotFound					; Yes, so error out
	
	test byte [es:di + 25], 0x02		; The entry is a directory?
	jz .NextDirEntry0					; No, so skip to the next
	
	cmp byte [es:di + 32], 4			; Same filename length?
	jne .NextDirEntry0					; No, so skip to the next
	
	push di								; Right folder?
	mov cx, 4
	mov si, FolderName
	add di, 33
	cld
	rep cmpsb
	pop di
	je .FolderFound
.NextDirEntry0:
	add di, ax							; No, go to the next directory entry
	cmp di, 0xD00
	jb .ProcessDirEntry0
.NextSector:
	inc bx								; Advance to the next directory sector
	jmp .ProcessSector0
.FolderFound:
	mov bx, [es:di + 2]					; Found! Get the starting sector
	mov cx, [es:di + 10]				; Get size
	add cx, 2047						; Convert to sectors
	shr cx, 11
	call ReadSectors					; Read the directory to memory!
.ProcessSector1:
	mov cx, 1							; Just one sector at time
	mov di, 0x500						; Read into 0x500
	call ReadSectors					; Load the current dir sector into buffer
.ProcessDirEntry1:
	xor ax, ax							; Terminator?
	mov al, [es:di]
	cmp al, 0
	je .FileNotFound					; Yes, so error out
	
	test byte [es:di + 25], 0x02		; The entry is a file?
	jnz .NextDirEntry1					; No, so skip to the next
	
	cmp byte [es:di + 32], 11			; Same filename length?
	jne .NextDirEntry1					; No, so skip to the next
	
	push di								; Right file?
	mov cx, 11
	mov si, FileName
	add di, 33
	cld
	rep cmpsb
	pop di
	je .LoaderFound
.NextDirEntry1:
	add di, ax							; No, go to next directory entry
	cmp di, 0xD00
	jb .ProcessDirEntry1
.NextSector1:
	inc bx								; Advance to next directory sector
	jmp .ProcessSector1
.LoaderFound:
	mov bx, [es:di + 2]					; Found! Get the starting sector
	mov cx, [es:di + 10]				; Get size
	add cx, 2047						; Convert to sectors
	shr cx, 11
	mov ax, 0x50						; Initialize es:di
	mov es, ax
	xor di, di
	call ReadSectors					; Read loader to memory
	
	mov dl, [BootDrive]					; Pop back the boot drive to dl
	jmp 0x00:0x500						; Jump to loader
.Halt:
	hlt
	jmp .Halt

.NotFound:
	mov si, RootDirErrMsg				; Print err to screen
	call ConWriteString
	call AnyKeyRestart
.FolderNotFound:
	mov si, FolderNotFoundMsg
	call ConWriteString
	call AnyKeyRestart
.FileNotFound:
	mov si, FileNotFoundMsg				; Print err to screen
	call ConWriteString
	call AnyKeyRestart

ConWriteString:
	pusha
	mov ah, 0x0E						; 0x0E is int 0x10 print character
	xor bx, bx							; Both bl and bh should be 0
.Write:
	lodsb								; Get next character
	or al, al							; Is a termination (0)?
	jz .Return							; Yes, so return
	int 0x10							; No, so write it
	jmp .Write
.Return:
	popa
	ret

AnyKeyRestart:
	mov si, AnyKeyMsg					; Print any key message
	call ConWriteString
	int 0x16							; Wait for keypress
Restart:
	int 0x19
.Hang:
	hlt
	jmp .Hang

ReadSectors:							; Read sectors function for ISO9660
	pusha
	
	mov dl, byte [BootDrive]			; Restore boot drive to dl
	
	mov word [DAPBuffer + 2], cx		; Fill the DAP buffer
	mov word [DAPBuffer + 4], di
	mov word [DAPBuffer + 6], es
	mov word [DAPBuffer + 8], bx
	
	lea si, [DAPBuffer]					; Load ds:si with DAP buffer's address
	
	mov ax, 0x4200						; Now use BIOS extended read sectors
	int 0x13
	
	jc .Error							; Error?
	
	popa								; Nope, so return
	ret
.Error:
	mov si, BIOSErrMsg					; Yes...
	call ConWriteString
	call AnyKeyRestart

DAPBuffer:
	db 16								; DAP size = 16
	db 0								; Reserved
	dw 1								; Sector count
	dw 0								; Destination offset
	dw 0								; Destination segment
	dq 0								; Sector LBA

BootDrive:
	db 0

BIOSErrMsg: db "BIOS error!", 0x0D, 0x0A, 0
RootDirErrMsg: db "Could not find root directory", 0x0D, 0x0A, 0
FolderNotFoundMsg: db "Could not find the Boot folder", 0x0D, 0x0A, 0
FileNotFoundMsg: db "Could not find the chboot.bin file", 0x0D, 0x0A, 0
AnyKeyMsg: db "Press any key to restart", 0x0D, 0x0A, 0
FolderName: db "Boot"
FileName: db "bootmgr.bin"

times 510 - ($ - $$) db 0
dw 0xAA55
