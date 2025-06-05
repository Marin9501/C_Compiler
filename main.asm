%macro time 2
    dq %1
    dq %2
%endmacro

section .data
    message db "Enter n (max 4 digits): "
    m_len equ $ - message
    nl db 0xa
    n dq 0
    start_time: time 0, 0
    end_time: time 0, 0

section .bss
    user_n resb 5
    ascii_n resb 100

section .text
    global _start
    
_start:
    ;display message
    mov rax, 1
    mov rdi, 1
    mov rsi, message
    mov rdx, m_len
    syscall

    ;get user input
    mov rax, 0
    mov rdi, 0
    mov rsi, user_n
    mov rdx, 5
    syscall

;result in rbx
mov rbx, 0
mov rsi, user_n
xor r8, r8
str_to_int:
    cmp byte [rsi], 0xa
    je str_to_int_end

    mov r8b, byte [rsi]
    sub r8, '0'
    imul rbx, rbx, 10
    add rbx, r8

    inc rsi
    jmp str_to_int

str_to_int_end:
    mov [n], rbx

;get start time
mov rax, 228
mov rdi, 1
mov rsi, start_time
syscall

;r8 is counter
xor r8, r8
;r9 is the smaller number
xor r9, r9
;r10 is the bigger number
mov r10, 1
;r11 has intermediate values
xor r11, r11
loop_start:
    cmp r8, qword [n]
    jge loop_end

    mov r11, r10
    add r10, r9
    mov r9, r11

    inc r8
    jmp loop_start
loop_end:
    mov rax, r10
    mov rcx, 10
    mov r9, 100
int_to_str:
    xor rdx, rdx
    div rcx
    add dl, '0'
    mov [ascii_n+r9], dl
    dec r9

    cmp rax, 0
    je int_to_str_end

    jmp int_to_str

int_to_str_end:
    mov rax, 1
    mov rdi, 1
    mov rsi, ascii_n
    mov rdx, 100
    syscall

    mov rax, 1
    mov rdi, 1
    mov rsi, nl
    mov rdx, 1
    syscall

    ;exit
    mov rax, 60 
    mov rdi, 0
    syscall
    
