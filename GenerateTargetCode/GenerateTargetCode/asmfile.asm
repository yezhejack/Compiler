TITLE datatype.asm : example for datatypes
.386
.model flat, stdcall
.stack 2048
option casemap : none
includelib	msvcrt.lib
printf		proto c : ptr byte, : vararg
scanf		proto c : ptr byte, : vararg
_getche		proto c
ExitProcess 	proto : dword
chr$ MACRO any_text : VARARG
LOCAL txtname
.data
txtname byte any_text, 0
.code
EXITM <OFFSET txtname>
ENDM
.data
num1  dword   ?
num2  dword   ?
num3  dword   ?
flag  dword   ?
fl  dword   ?
t0  dword   ?
t1  dword   ?
t2  dword   ?
t3  dword   ?
t4  dword   ?
t5  dword   ?
t6  dword   ?
t7  dword   ?
t8  dword   ?
t9  dword   ?
t10  dword   ?
t11  dword   ?
t12  dword   ?
.code
main proc
label0:
     mov eax, 0
     mov t0 ,eax
label1:
     mov eax, t0
     mov flag ,eax
label2:
     mov eax, 0
     mov t1 ,eax
label3:
     mov eax, t1
     mov num1 ,eax
label4:
     mov eax, 100
     mov t2 ,eax
label5:
     mov eax, t2
     mov num2 ,eax
label6:
     mov eax, num2
     add eax, num2
     mov t3 ,eax
label7:
     mov eax, num1
     add eax, t3
     mov t4 ,eax
label8:
     mov eax, t4
     mov num3 ,eax
label9:
     mov eax, 0
     mov t5 ,eax
label10:
     mov eax,num2
     mov ebx,t5
     cmp eax,ebx
jeb label17
label11:
     mov eax, num1
     add eax, num2
     mov t6 ,eax
label12:
     mov eax, t6
     mov num1 ,eax
label13:
     mov eax, 1
     mov t7 ,eax
label14:
     mov eax, num2
     sub eax, t7
     mov t8 ,eax
label15:
     mov eax, t8
     mov num2 ,eax
label16:
     jmp label10
label17:
     mov eax, 0
     mov t9 ,eax
label18:
     mov eax,flag
     mov ebx,t9
     cmp eax,ebx
jne label23
label19:
     mov eax, 5
     mov t10 ,eax
label20:
     mov eax, num1
     mul t10
     mov t11 ,eax
label21:
     mov eax, t11
     mov num1 ,eax
label22:
     jmp label25
label23:
     mov eax, num3
     sub eax, num1
     mov t12 ,eax
label24:
     mov eax, t12
     mov num2 ,eax
label25:
     mov eax,num1
     invoke printf,chr$("%d"),eax
label26:
     invoke _getche
     invoke ExitProcess,0
main endp
end main
