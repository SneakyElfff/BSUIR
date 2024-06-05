.model small
.stack 100h

.data
error db "Nothing was read$"
buffer db ?
message db "Read data: $"
                          
.code
    mov ax, @data
    mov ds, ax
    
initialise:
    mov al, 80h    ;7th bit - 1 (Divisor Latch Access Bit) => 3F8h and 3F9h - speed
    mov dx, 3FBh    ;Line Control Register
    out dx, al
    
    mov dx, 3F8h    ;controlling register, in charge of transmitting data. "low" byte
    mov al, 00h    ;0 - port is used for writing data
    out dx, al
    
    mov al, 0Ch    ;"high" byte, 0Ch - 12 - 9600 (speed)
    mov dx, 3F9h    ;register controlls interruptions
    out dx, al
    
    mov dx, 3FCh    ;port controls the modem
    mov al, 00001011b    ;DTR, RTS, OUT1, OUT2
    out dx, al
    
    mov dx, 3F9h
    mov al, 0
    out dx, al 
    
write:    ;using COM1
    xor al, al
    mov dx, 3FDh
    in al, dx
    
    mov dx, 3F8h
    mov al, 'c'
    out dx, al
    
read:    ;using COM2
    xor al, al
    mov dx, 2FDh    ;ready for reading data
    in al, dx
    
    mov dx, 2F8h
    in al, dx
    mov buffer, al
   
output:
    cmp buffer, 0
    je not_read
    
    lea dx, message
    mov ah, 09h
    int 21h
    
    mov ah, 02h
    xor dx, dx
    mov dl, buffer
    int 21h
    
exit:
    mov ax, 4C00h
    int 21h
    ret
    
not_read:
    lea dx, error
    mov ah, 09h
    int 21h
