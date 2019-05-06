global rec

section .text
rec:
  push rbp
  mov  rbp, rsp
  push rbx
  mov  rax, rdi 
  cmp  rax, 0
  je   zero
  cmp  rax, 1
  je   one
  
  dec  rax
  push rax
  mov  rdi, rax
  call rec
  mov  rbx, rax
  pop  rax
  
  dec  rax
  push rax
  mov  rdi, rax
  call rec
  add  rbx, rax
  pop  rax
  
  mov  rax, rbx
  jmp  end

zero:
  mov  rax, 0
  jmp  end

one:
  mov  rax, 1
  jmp  end

end:
  pop  rbx
  pop  rbp
  ret

