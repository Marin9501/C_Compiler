section .data
    message1 db "Enter a number max 7 digits: "
    m1_len equ $ - message1

    message2 db 0xa, "-th Fibonachi number is: "
    m2_len equ $ - message2

    temp db "Loop iter", 0xa

section .bss
    n1 resb 2
    int_n1 resq 1
    
section .text
    global _start

    ; ;display message
    ; mov rax, 1
    ; mov rdi, 1
    ; mov rsi, message1
    ; mov rdx, m1_len
    ; syscall
    ;
    ; ;read the user number
    ; mov rax, 0
    ; mov rdi, 0
    ; mov rsi, n1
    ; mov rdx, 8
    ; syscall
    ;
    ; ;display the number
    ; mov rax, 1
    ; mov rdi, 1
    ; mov rsi, n1
    ; mov rdx, 8
    ; syscall

    ;convert the number to int
    ;result in rbx
    ; mov rbx, 0
    ; mov rsi, n1
    ; convert_to_int:
    ;     movzx rax, byte [rsi]
    ;     cmp rax, 0xa
    ;     je end_convert_to_int
    ;
    ;     sub rax, '0'
    ;     imul rbx, rbx, 10
    ;     add rbx, rax
    ;
    ;     inc rsi
    ;     jmp convert_to_int
    ; end_convert_to_int:

; init the loop counter
_start:
    mov rcx, 0
loop_start:
    cmp rcx, 9
    jge loop_end

    mov [n1], rcx
    add byte [n1],'0'
    mov rax, 1
    mov rdi, 1
    mov rsi, n1
    mov rdx, 1
    syscall

    mov rax, 1
    mov rdi, 1
    mov rsi, temp
    mov rdx, 10
    syscall

    inc rcx
    mov byte [n1], cl
    add byte [n1],'0'
    mov rax, 1
    mov rdi, 1
    mov rsi, n1
    mov rdx, 1
    syscall
    jmp loop_start
loop_end:
    ;exit
    mov rax, 60
    mov rdi, 0
    syscall

