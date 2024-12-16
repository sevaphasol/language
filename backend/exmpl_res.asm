push 1
pop her
push 1
pop penis
push 2
pop BX
call main:
hlt

huy:
pop [BP + 0]
push v
push 1
add
out
push 0
push 1
jne label:
push 1
out
label:
ret

main:
pop [BP + 0]
pop [BP + 1]
push 1
pop d
in
pop a
push BP
call huy:
push a
out
ret

