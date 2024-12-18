push 0
pop BP
call main:
hlt

;declaration of function factorial
factorial:
;pushing if params
;pushing local var n
push [BP + 0]
push 0
je else_body_0
;pushing local var n
push [BP + 0]
; save BP
push BP
; pushing func_params
;pushing local var n
push [BP + 0]
push 1
sub
;set BP
push BP
push 0
add
; poping func_params to local variables
pop [BP + 0]
; calling function
call factorial:
pop  BP
push AX
mul
pop AX
ret

else_body_0:
push 1
pop AX
ret

;declaration of function main
main:
push 0
pop [BP + 0]
in
pop [BP + 0]
; save BP
push BP
; pushing func_params
;pushing local var n
push [BP + 0]
;set BP
push BP
push 0
add
; poping func_params to local variables
pop [BP + 0]
; calling function
call factorial:
pop  BP
push AX
out
pop AX
ret

