.model small
.stack 100h

.data
error db "Nothing was read$"
message db "Read data: $"

.code
    mov ax, @data
    mov ds, ax

initialise:
    mov ah, 00h    ;function initialises the COM-port
    mov al, 11100011b    ;111 - 9600 (speed), 00 - no parity, 0 - 1 stopbit, 11 - 8 bits  
    int 14h
    
write:
    mov al, 'c'
    mov ah, 01h    ;function writes a symbol to COM-port
    mov dx, 0    ;the first COM-port (n=1, n-1=0)
    int 14h
    
read:
    mov ah, 02h    ;function reads a symbol from COM-port
    mov dx, 1    ;the second COM-port
    int 14h
    
output:
    cmp al, 0
    je not_read
    
    lea dx, message
    mov ah, 09h
    int 21h
    
    mov ah, 02h
    xor dx, dx
    mov dl, al
    int 21h
    
exit:
    mov ax, 4c00h
    int 21h
    ret
    
not_read:
    lea dx, error
    mov ah, 09h
    int 21h
    
    
    
