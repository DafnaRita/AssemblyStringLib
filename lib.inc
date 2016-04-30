string_length: ;
    xor rax, rax
    xor rcx, rcx
    .start_loop:   
	cmp byte[rdi], 0      
            jz .jump_end      
	inc rdi     
	inc rax      
	jmp .start_loop    
    .jump_end:
ret

print_string: ;
    mov r8 , rdi
    call string_length
    mov rdx, rax
    mov rax, 1 
    mov rdi, 1 
    mov rsi, r8
    syscall	
ret

print_char:
    push rdi
    mov rax, 1
    mov rdi, 1
    mov rsi, rsp
    mov rdx, 1
    syscall
    pop rdi
ret

print_uint:
    push r12
    mov r12, rsp
    sub rsp,  30   ; allocate
    dec r12
    mov byte[r12], 0
    dec r12
    mov rax, rdi
    mov rsi, 10    ; division
    ._start_divide:    ; rdx - remainder, rax - quotient
        xor rdx, rdx
        div rsi
        add rdx, '0'
        mov byte[r12], dl
        cmp rax, 0
            jz .stop_divide
        dec r12
        jmp ._start_divide
	.stop_divide:
            mov rdi, r12
            call print_string
            add rsp,  30 
            pop r12	
ret

print_newline: 
    mov rdi, 0
    call print_char
ret

print_int:
    cmp rdi, 0
        js .negative
        call print_uint   ; positive
        jmp ._end
    .negative:
        mov r12, rdi
        mov rdi, '-'
        call print_char
        neg r12
        mov rdi, r12
        call print_uint 
    ._end:
ret

	
string_equals:  ; rdi , rsi - строки
    xor rax, rax
    .start_loop:
        mov r13b, [rdi]
        cmp r13b, [rsi]
            jne .fail
        cmp byte[rdi], 0   ; check for null-terminate
            je .equals
        inc rdi
        inc rsi
        jmp .start_loop
    .fail:
        mov rax, 0
        jmp .end_equals
    .equals:
        mov rax, 1
        jmp .end_equals
    .end_equals:
ret

read_char: 
    push 0
    mov rax, 0 ; 
    mov rdi, 0
    mov rsi, rsp ;  
    mov rdx, 1
    syscall
    pop rax
ret
	
section .data
word_buffer times 256 db 0

section .text
	
read_word:
    xor r12, r12  
    push r13
    mov r13, word_buffer   ; save the adress of buffer
    .start_loop:	
        call read_char
        cmp rax , 0    
            je .end_string           
        cmp rax, 9
            je .start_loop
        cmp rax, 10
            je .start_loop
        cmp rax, ' '		
            je .start_loop	
        cmp rax, 13
            je .start_loop
        mov [r13], rax     ; starting of our word
        inc r13
        inc r12
    .start_loop_word:
        call read_char
        cmp rax , 0
            je .end_string  
            jmp .prov_na_0
    .prov_na_0:
    cmp rax , ' '
        je .end_string  
    jmp .prov_na_space
    .prov_na_space:
        mov [r13], rax
        inc r13
        inc r12
        jmp .start_loop_word
    .end_string:
    mov rax, word_buffer
    mov rdx, r12
    pop r13
ret
	
;rdx=r15 
; returns rax: number, rdx : length(r15)

parse_uint:
    xor rax, rax
    xor rdx, rdx
    push r14
    xor r14, r14
    push r12
    push r13		
    mov r12, 10    ; multiplier
    ._start:
        movzx r13, byte [rdi]  ; r13b is the buffer, get the number
        cmp r13b, '0'          
            jb .end_of_loop
        cmp r13b, '9'	
            ja .end_of_loop
        inc r14    ; got the number
    mul r12 ; mul rdx:rax=rax*10
        sub r13b, '0'
        add rax, r13   ; add the number to rax
        inc rdi    ; inc our pointer
        jmp ._start
    .end_of_loop: 	
    pop r13
    pop r12
    pop r14
ret 

; rdi points to a string
; returns rax: number, rdx : length

parse_int:
    xor rax, rax
    xor rdx, rdx
    xor rsi, rsi
    mov r12, 10    ; multiplier
    .getting_digit:
        movzx r13, byte [rdi]   ; r13b is the buffer, get the number
        cmp r13b, '-'
            je .negative	
        cmp r13b, 0X30            
            jb .parse_int_loop
        cmp r13b, 0X39	
            ja .parse_int_loop
    sub r13b, 48
    mul r12    ; mul rdx:rax=rax*10	
    add rax, r13    ; add the number to rax
    inc rdx    ; got the number
    inc rdi    ; inc our pointer
    jmp .getting_digit
    .negative:
        mov rsi, 1    ; negative
        inc rdi
        jmp .getting_digit
    .setNegativ:
        neg rax
        jmp .parse_int_end
    .parse_int_loop:
        cmp rsi, 1
            je .setNegativ
    .parse_int_end:    ;ending	   	
ret 

string_copy:    ;rdi - this , rsi - to  
    .copy_loop:	
	mov al, byte[rdi]
	mov byte[rsi], al
	cmp al, 0
            je .loop_end
	inc rsi
	inc rdi
	jmp .copy_loop
    .loop_end:
ret
	
sum: ;rax - number
    ocn:    mov rdx, rax
    loop:   cmp rax, 0
        je equals
        jmp not_equals
    equals:
        mov rax, rdx
        ret
    not_equals:
        dec rax
        add rdx, rax
        jmp loop