STACKSG SEGMENT PARA STACK 'STACK'
    DW 128 DUP(?)
STACKSG ENDS

DATASG  SEGMENT PARA 'DATA'
    SONUC DW 0
    N DW 10
    MSG DB 'Sonuc: $'
DATASG ENDS

CODESG  SEGMENT PARA 'CODE'
        ASSUME CS:CODESG, DS:DATASG, SS:STACKSG

BASLA   PROC FAR
        PUSH DS
        XOR AX,AX
        PUSH AX
        MOV AX, DATASG
        MOV DS, AX

        PUSH N
        call far ptr DNUM
        pop SONUC

        PUSH SONUC
        call far ptr PRINTINT

        RETF
BASLA   ENDP

PRINTINT PROC FAR
	push bp
    push ax
    push bx
    push cx
    push dx
    MOV AH,09 ;Ekrana 'Sonuc: ' yazdirmak icin
	LEA DX,MSG
	INT 21H
	mov bp, sp
	mov ax, [bp+14] ;ax = sonuc
	mov bx, 10 
	mov cx, 0
	
printint_loop:
	XOR dx,dx
	div bx	;ax = sonuc / 10, dx = sonuc % 10
	add dl, '0' ;dl = sonuc % 10 + '0'
	push dx 
	inc cx ;cx = basamak sayisi
	cmp ax, 0
	jne printint_loop
	
printint_print:
	pop dx
	mov ah, 02h
	int 21h ;Ekrana basamak sayisi kadar rakam yazdirma
	loop printint_print
	pop dx
    pop cx
    pop bx
    pop ax
	pop bp
	retf 2 
PRINTINT ENDP

DNUM PROC FAR
	push bp
    push ax
    push bx
    push cx
    push dx
	mov bp, sp
	mov ax, [bp+14] ; ax = N
	cmp ax, 0
	jne dnum_check
	mov word ptr [bp+14] , 0
	jmp dnum_exit
dnum_check:
	cmp ax, 1
	jne dnum_check2
	mov word ptr [bp+14] , 1
	jmp dnum_exit
dnum_check2:
	cmp ax, 2
	jne dnum_calc
	mov word ptr [bp+14] , 1
	jmp dnum_exit
dnum_calc:
    dec ax
	push ax
	call far ptr DNUM; D(n-1)
	call far ptr DNUM; D(D(n-1))
	pop bx
	dec ax
    push ax
	call far ptr DNUM; D(n-2)
    pop cx
    inc ax
    mov dx,ax
	sub dx, cx
    push dx
    call far ptr DNUM; D(n-1-D(n-2)
    pop cx
	add bx, cx
    mov [bp+14] , bx; Sonuc = bx
dnum_exit:
    pop dx
    pop cx
    pop bx
    pop ax
	pop bp
	RETF
DNUM ENDP

CODESG  ENDS
        END BASLA