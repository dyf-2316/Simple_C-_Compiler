	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 10, 15, 4	sdk_version 10, 15, 4
	.globl	_main                   ## -- Begin function main
	.p2align	4, 0x90
_main:                                  ## @main
	.cfi_startproc
## %bb.0:
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset %ebp, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register %ebp
	calll	L0$pb
L0$pb:
	popl	%eax
	xorl	%ecx, %ecx
	movl	_a-L0$pb(%eax), %edx
	addl	$1, %edx
	movl	%edx, _a-L0$pb(%eax)
	movl	%ecx, %eax
	popl	%ebp
	retl
	.cfi_endproc
                                        ## -- End function
	.globl	_a                      ## @a
.zerofill __DATA,__common,_a,4,2

.subsections_via_symbols
