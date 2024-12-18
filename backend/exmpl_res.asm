push 0
pop BP
call main:
hlt

factorial:
    push [BP + 0]
    push 0
    je else_body_0:
        push [BP + 0]
        push BP
        push [BP + 0]
        push 1
        sub
        push BP
        push 0
        add
        pop [BP + 0]
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
main:
    push 5
    pop [BP + 0]
    in
    pop [BP + 0]
    push BP
    push [BP + 0]
    push BP
    push 1
    add
    pop [BP + 0]
    call factorial:
    pop  BP
    push AX
    out
    pop AX
    ret
