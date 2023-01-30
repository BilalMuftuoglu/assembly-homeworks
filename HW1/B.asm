STACKSG SEGMENT PARA STACK 'STACK'
    DW 128 DUP(?)
STACKSG ENDS

DATASG  SEGMENT PARA 'DATA'
    SONUC DW 0
    N DW 500
    MSG DB 'Sonuc: $'
    DIZI DW 501 DUP(-1); video kaydını aldıktan sonra dizi başlangıç değerini -1 olarak güncelledim
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
	div bx ;ax = sonuc / 10, dx = sonuc % 10
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
    push si
    
    lea si,DIZI
	mov bp, sp
	mov bx, [bp+16] ; bx = N
    shl bx,1
    mov ax, [si+bx] ;ax = DIZI[n]
    shr bx,1
    cmp ax, -1 ; DIZI'den aldığımız değerin kontrolü
    jne fin

	cmp bx, 0
	jne dnum_check
	mov word ptr [bp+16] , 0
	jmp dnum_exit
dnum_check:
	cmp bx, 1
	jne dnum_check2
	mov word ptr [bp+16] , 1
	jmp dnum_exit
dnum_check2:
	cmp bx, 2
	jne dnum_calc
	mov word ptr [bp+16] , 1
	jmp dnum_exit
dnum_calc:
    dec bx
	push bx
	call far ptr DNUM ; D(n-1)
	call far ptr DNUM ; D(D(n-1))
	pop ax
	dec bx
    push bx
	call far ptr DNUM ; D(n-2)
    pop cx
    inc bx
    mov dx,bx
	sub dx, cx
    push dx
    call far ptr DNUM ; D(n-1-D(n-2)
    pop cx
	add ax, cx
    inc bx
    shl bx,1
    mov [si+bx] , ax ; DIZI[n] = ax
    shr bx,1
fin:
    mov [bp+16] , ax ; Sonuc = ax
dnum_exit:
    pop si
    pop dx
    pop cx
    pop bx
    pop ax
	pop bp
	RETF
DNUM ENDP

CODESG  ENDS
        END BASLA