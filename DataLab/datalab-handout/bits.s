	.file	"bits.c"
	.text
	.globl	changeHigh
	.type	changeHigh, @function
changeHigh:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -36(%rbp)
	movl	%esi, -40(%rbp)
	movl	%edx, -44(%rbp)
	movl	-36(%rbp), %eax
	sall	$16, %eax
	movl	%eax, %edx
	movl	-40(%rbp), %eax
	sall	$16, %eax
	cmpl	%eax, %edx
	setne	%al
	movzbl	%al, %eax
	movl	%eax, -20(%rbp)
	movl	$-1, -16(%rbp)
	movl	-16(%rbp), %eax
	sall	$16, %eax
	movl	%eax, -12(%rbp)
	movl	-16(%rbp), %eax
	addl	$65536, %eax
	movl	%eax, -8(%rbp)
	movl	-36(%rbp), %eax
	andl	-12(%rbp), %eax
	movl	%eax, %edx
	movl	-40(%rbp), %eax
	sarl	$16, %eax
	andl	-8(%rbp), %eax
	orl	%edx, %eax
	movl	%eax, -4(%rbp)
	movl	-20(%rbp), %eax
	sall	$4, %eax
	movl	%eax, %ecx
	sall	%cl, -4(%rbp)
	movl	-4(%rbp), %eax
	andl	-12(%rbp), %eax
	movl	%eax, %edx
	movl	-44(%rbp), %eax
	andl	-8(%rbp), %eax
	orl	%edx, %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	changeHigh, .-changeHigh
	.globl	newOp
	.type	newOp, @function
newOp:
.LFB1:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	notl	%eax
	andl	-8(%rbp), %eax
	notl	%eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	newOp, .-newOp
	.globl	onlyOne
	.type	onlyOne, @function
onlyOne:
.LFB2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -20(%rbp)
	movl	%esi, -24(%rbp)
	movl	-20(%rbp), %eax
	xorl	-24(%rbp), %eax
	movl	%eax, -8(%rbp)
	movl	-8(%rbp), %eax
	subl	$1, %eax
	andl	-8(%rbp), %eax
	movl	%eax, -4(%rbp)
	cmpl	$0, -8(%rbp)
	sete	%al
	movzbl	%al, %eax
	addl	%eax, -4(%rbp)
	cmpl	$0, -4(%rbp)
	sete	%al
	movzbl	%al, %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	onlyOne, .-onlyOne
	.globl	truncAvg
	.type	truncAvg, @function
truncAvg:
.LFB3:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -36(%rbp)
	movl	%esi, -40(%rbp)
	movl	-36(%rbp), %eax
	sarl	%eax
	movl	%eax, -32(%rbp)
	movl	-40(%rbp), %eax
	sarl	%eax
	movl	%eax, -28(%rbp)
	movl	-32(%rbp), %edx
	movl	-28(%rbp), %eax
	addl	%edx, %eax
	movl	%eax, -24(%rbp)
	movl	-24(%rbp), %eax
	sarl	$31, %eax
	movl	%eax, -20(%rbp)
	movl	-36(%rbp), %eax
	andl	$1, %eax
	movl	%eax, -16(%rbp)
	movl	-40(%rbp), %eax
	andl	$1, %eax
	movl	%eax, -12(%rbp)
	cmpl	$0, -20(%rbp)
	setne	%al
	movzbl	%al, %eax
	movl	%eax, -8(%rbp)
	movl	-16(%rbp), %edx
	movl	-12(%rbp), %eax
	addl	%eax, %edx
	movl	-8(%rbp), %eax
	addl	%edx, %eax
	sarl	%eax
	movl	%eax, -4(%rbp)
	movl	-24(%rbp), %edx
	movl	-4(%rbp), %eax
	addl	%edx, %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3:
	.size	truncAvg, .-truncAvg
	.globl	absVal
	.type	absVal, @function
absVal:
.LFB4:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -20(%rbp)
	movl	-20(%rbp), %eax
	sarl	$31, %eax
	movl	%eax, -4(%rbp)
	movl	-20(%rbp), %eax
	xorl	-4(%rbp), %eax
	movl	-4(%rbp), %edx
	andl	$1, %edx
	addl	%edx, %eax
	movl	%eax, -20(%rbp)
	movl	-20(%rbp), %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE4:
	.size	absVal, .-absVal
	.globl	subOK
	.type	subOK, @function
subOK:
.LFB5:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -36(%rbp)
	movl	%esi, -40(%rbp)
	cmpl	$-2147483648, -40(%rbp)
	sete	%al
	movzbl	%al, %eax
	movl	%eax, -28(%rbp)
	movl	-40(%rbp), %eax
	notl	%eax
	movl	%eax, %edx
	cmpl	$0, -28(%rbp)
	sete	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, -24(%rbp)
	movl	-36(%rbp), %eax
	orl	-24(%rbp), %eax
	sarl	$31, %eax
	notl	%eax
	movl	%eax, -20(%rbp)
	movl	-36(%rbp), %eax
	andl	-24(%rbp), %eax
	sarl	$31, %eax
	movl	%eax, -16(%rbp)
	movl	-36(%rbp), %edx
	movl	-24(%rbp), %eax
	addl	%eax, %edx
	movl	-28(%rbp), %eax
	addl	%edx, %eax
	movl	%eax, -12(%rbp)
	movl	-12(%rbp), %eax
	sarl	$31, %eax
	andl	-20(%rbp), %eax
	movl	%eax, -8(%rbp)
	movl	-12(%rbp), %eax
	sarl	$31, %eax
	notl	%eax
	andl	-16(%rbp), %eax
	movl	%eax, -4(%rbp)
	movl	-8(%rbp), %edx
	movl	-4(%rbp), %eax
	addl	%edx, %eax
	testl	%eax, %eax
	sete	%al
	movzbl	%al, %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5:
	.size	subOK, .-subOK
	.globl	fullAdd
	.type	fullAdd, @function
fullAdd:
.LFB6:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -52(%rbp)
	movl	%esi, -56(%rbp)
	movl	-52(%rbp), %eax
	xorl	-56(%rbp), %eax
	movl	%eax, -36(%rbp)
	movl	-52(%rbp), %eax
	andl	-56(%rbp), %eax
	addl	%eax, %eax
	movl	%eax, -32(%rbp)
	movl	-32(%rbp), %eax
	andl	$2, %eax
	movl	%eax, -28(%rbp)
	movl	-36(%rbp), %eax
	xorl	-28(%rbp), %eax
	movl	%eax, -24(%rbp)
	movl	-36(%rbp), %eax
	andl	-28(%rbp), %eax
	addl	%eax, %eax
	movl	%eax, -20(%rbp)
	movl	-20(%rbp), %eax
	orl	%eax, -32(%rbp)
	movl	-32(%rbp), %eax
	andl	$4, %eax
	movl	%eax, -16(%rbp)
	movl	-16(%rbp), %eax
	xorl	-24(%rbp), %eax
	movl	%eax, -12(%rbp)
	movl	-36(%rbp), %eax
	andl	-16(%rbp), %eax
	addl	%eax, %eax
	movl	%eax, -20(%rbp)
	movl	-20(%rbp), %eax
	orl	%eax, -32(%rbp)
	movl	-32(%rbp), %eax
	andl	$8, %eax
	movl	%eax, -8(%rbp)
	movl	-8(%rbp), %eax
	xorl	-12(%rbp), %eax
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	fullAdd, .-fullAdd
	.globl	theMax
	.type	theMax, @function
theMax:
.LFB7:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -36(%rbp)
	movl	%esi, -40(%rbp)
	movl	-36(%rbp), %eax
	sarl	$31, %eax
	movl	%eax, -24(%rbp)
	movl	-40(%rbp), %eax
	sarl	$31, %eax
	movl	%eax, -20(%rbp)
	movl	-24(%rbp), %eax
	xorl	-20(%rbp), %eax
	movl	%eax, -16(%rbp)
	movl	-24(%rbp), %eax
	notl	%eax
	andl	-16(%rbp), %eax
	notl	%eax
	movl	%eax, -12(%rbp)
	movl	-20(%rbp), %eax
	notl	%eax
	andl	-16(%rbp), %eax
	movl	%eax, -8(%rbp)
	movl	-40(%rbp), %eax
	notl	%eax
	movl	%eax, %edx
	movl	-36(%rbp), %eax
	addl	%edx, %eax
	addl	$1, %eax
	sarl	$31, %eax
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	orl	-8(%rbp), %eax
	andl	-12(%rbp), %eax
	movl	%eax, -4(%rbp)
	movl	-40(%rbp), %eax
	andl	-4(%rbp), %eax
	movl	%eax, %edx
	movl	-36(%rbp), %eax
	orl	-4(%rbp), %eax
	addl	%eax, %edx
	cmpl	$0, -4(%rbp)
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	theMax, .-theMax
	.globl	palindrome
	.type	palindrome, @function
palindrome:
.LFB8:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -84(%rbp)
	movl	-84(%rbp), %eax
	sall	$31, %eax
	movl	%eax, -68(%rbp)
	movl	-84(%rbp), %eax
	sall	$29, %eax
	andl	$1073741824, %eax
	movl	%eax, -64(%rbp)
	movl	-84(%rbp), %eax
	sall	$27, %eax
	andl	$536870912, %eax
	movl	%eax, -60(%rbp)
	movl	-84(%rbp), %eax
	sall	$25, %eax
	andl	$268435456, %eax
	movl	%eax, -56(%rbp)
	movl	-84(%rbp), %eax
	sall	$23, %eax
	andl	$134217728, %eax
	movl	%eax, -52(%rbp)
	movl	-84(%rbp), %eax
	sall	$21, %eax
	andl	$67108864, %eax
	movl	%eax, -48(%rbp)
	movl	-84(%rbp), %eax
	sall	$19, %eax
	andl	$33554432, %eax
	movl	%eax, -44(%rbp)
	movl	-84(%rbp), %eax
	sall	$17, %eax
	andl	$16777216, %eax
	movl	%eax, -40(%rbp)
	movl	-84(%rbp), %eax
	sall	$15, %eax
	andl	$8388608, %eax
	movl	%eax, -36(%rbp)
	movl	-84(%rbp), %eax
	sall	$13, %eax
	andl	$4194304, %eax
	movl	%eax, -32(%rbp)
	movl	-84(%rbp), %eax
	sall	$11, %eax
	andl	$2097152, %eax
	movl	%eax, -28(%rbp)
	movl	-84(%rbp), %eax
	sall	$9, %eax
	andl	$1048576, %eax
	movl	%eax, -24(%rbp)
	movl	-84(%rbp), %eax
	sall	$7, %eax
	andl	$524288, %eax
	movl	%eax, -20(%rbp)
	movl	-84(%rbp), %eax
	sall	$5, %eax
	andl	$262144, %eax
	movl	%eax, -16(%rbp)
	movl	-84(%rbp), %eax
	sall	$3, %eax
	andl	$131072, %eax
	movl	%eax, -12(%rbp)
	movl	-84(%rbp), %eax
	addl	%eax, %eax
	andl	$65536, %eax
	movl	%eax, -8(%rbp)
	movl	-68(%rbp), %eax
	orl	-64(%rbp), %eax
	orl	-60(%rbp), %eax
	orl	-56(%rbp), %eax
	orl	-52(%rbp), %eax
	orl	-48(%rbp), %eax
	orl	-44(%rbp), %eax
	orl	-40(%rbp), %eax
	orl	-36(%rbp), %eax
	orl	-32(%rbp), %eax
	orl	-28(%rbp), %eax
	orl	-24(%rbp), %eax
	orl	-20(%rbp), %eax
	orl	-16(%rbp), %eax
	orl	-12(%rbp), %eax
	orl	-8(%rbp), %eax
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	xorl	-84(%rbp), %eax
	movw	$0, %ax
	testl	%eax, %eax
	sete	%al
	movzbl	%al, %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	palindrome, .-palindrome
	.globl	distance
	.type	distance, @function
distance:
.LFB9:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -36(%rbp)
	movl	%esi, -40(%rbp)
	movl	-36(%rbp), %eax
	xorl	-40(%rbp), %eax
	movl	%eax, -20(%rbp)
	movl	$4369, -16(%rbp)
	movl	-16(%rbp), %eax
	sall	$16, %eax
	orl	%eax, -16(%rbp)
	movl	-20(%rbp), %eax
	andl	-16(%rbp), %eax
	movl	%eax, -12(%rbp)
	movl	-20(%rbp), %eax
	sarl	%eax
	andl	-16(%rbp), %eax
	addl	%eax, -12(%rbp)
	movl	-20(%rbp), %eax
	sarl	$2, %eax
	andl	-16(%rbp), %eax
	addl	%eax, -12(%rbp)
	movl	-20(%rbp), %eax
	sarl	$3, %eax
	andl	-16(%rbp), %eax
	addl	%eax, -12(%rbp)
	movl	$15, -8(%rbp)
	movl	-12(%rbp), %eax
	andl	-8(%rbp), %eax
	movl	%eax, -4(%rbp)
	movl	-12(%rbp), %eax
	sarl	$4, %eax
	andl	-8(%rbp), %eax
	addl	%eax, -4(%rbp)
	movl	-12(%rbp), %eax
	sarl	$8, %eax
	andl	-8(%rbp), %eax
	addl	%eax, -4(%rbp)
	movl	-12(%rbp), %eax
	sarl	$12, %eax
	andl	-8(%rbp), %eax
	addl	%eax, -4(%rbp)
	movl	-12(%rbp), %eax
	sarl	$16, %eax
	andl	-8(%rbp), %eax
	addl	%eax, -4(%rbp)
	movl	-12(%rbp), %eax
	sarl	$20, %eax
	andl	-8(%rbp), %eax
	addl	%eax, -4(%rbp)
	movl	-12(%rbp), %eax
	sarl	$24, %eax
	andl	-8(%rbp), %eax
	addl	%eax, -4(%rbp)
	movl	-12(%rbp), %eax
	sarl	$28, %eax
	andl	-8(%rbp), %eax
	addl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE9:
	.size	distance, .-distance
	.globl	divSix
	.type	divSix, @function
divSix:
.LFB10:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -68(%rbp)
	cmpl	$-2147483648, -68(%rbp)
	sete	%al
	movzbl	%al, %eax
	movl	%eax, -64(%rbp)
	movl	-68(%rbp), %eax
	sarl	$31, %eax
	movl	%eax, -60(%rbp)
	movl	$0, -56(%rbp)
	movl	-68(%rbp), %eax
	xorl	-60(%rbp), %eax
	movl	-60(%rbp), %edx
	andl	$1, %edx
	addl	%edx, %eax
	movl	%eax, -52(%rbp)
	movl	$1431655765, -48(%rbp)
	movl	-48(%rbp), %eax
	addl	%eax, %eax
	movl	%eax, -44(%rbp)
	movl	-52(%rbp), %eax
	andl	-48(%rbp), %eax
	movl	%eax, -40(%rbp)
	movl	-52(%rbp), %eax
	andl	-44(%rbp), %eax
	movl	%eax, -36(%rbp)
	movl	$0, -32(%rbp)
	movl	$0, -28(%rbp)
	movl	$0, -24(%rbp)
	movl	$-1, -20(%rbp)
	movl	-44(%rbp), %eax
	orl	$1073741824, %eax
	addl	$1, %eax
	movl	%eax, -16(%rbp)
	cmpl	$0, -40(%rbp)
	setne	%al
	movzbl	%al, %eax
	addl	%eax, -32(%rbp)
	movl	-40(%rbp), %edx
	movl	-20(%rbp), %eax
	addl	%edx, %eax
	andl	%eax, -40(%rbp)
	cmpl	$0, -40(%rbp)
	setne	%al
	movzbl	%al, %eax
	addl	%eax, -32(%rbp)
	movl	-40(%rbp), %edx
	movl	-20(%rbp), %eax
	addl	%edx, %eax
	andl	%eax, -40(%rbp)
	cmpl	$0, -40(%rbp)
	setne	%al
	movzbl	%al, %eax
	addl	%eax, -32(%rbp)
	movl	-40(%rbp), %edx
	movl	-20(%rbp), %eax
	addl	%edx, %eax
	andl	%eax, -40(%rbp)
	cmpl	$0, -40(%rbp)
	setne	%al
	movzbl	%al, %eax
	addl	%eax, -32(%rbp)
	movl	-40(%rbp), %edx
	movl	-20(%rbp), %eax
	addl	%edx, %eax
	andl	%eax, -40(%rbp)
	cmpl	$0, -40(%rbp)
	setne	%al
	movzbl	%al, %eax
	addl	%eax, -32(%rbp)
	movl	-40(%rbp), %edx
	movl	-20(%rbp), %eax
	addl	%edx, %eax
	andl	%eax, -40(%rbp)
	cmpl	$0, -40(%rbp)
	setne	%al
	movzbl	%al, %eax
	addl	%eax, -32(%rbp)
	movl	-40(%rbp), %edx
	movl	-20(%rbp), %eax
	addl	%edx, %eax
	andl	%eax, -40(%rbp)
	cmpl	$0, -40(%rbp)
	setne	%al
	movzbl	%al, %eax
	addl	%eax, -32(%rbp)
	movl	-40(%rbp), %edx
	movl	-20(%rbp), %eax
	addl	%edx, %eax
	andl	%eax, -40(%rbp)
	cmpl	$0, -40(%rbp)
	setne	%al
	movzbl	%al, %eax
	addl	%eax, -32(%rbp)
	movl	-40(%rbp), %edx
	movl	-20(%rbp), %eax
	addl	%edx, %eax
	andl	%eax, -40(%rbp)
	cmpl	$0, -40(%rbp)
	setne	%al
	movzbl	%al, %eax
	addl	%eax, -32(%rbp)
	movl	-40(%rbp), %edx
	movl	-20(%rbp), %eax
	addl	%edx, %eax
	andl	%eax, -40(%rbp)
	cmpl	$0, -40(%rbp)
	setne	%al
	movzbl	%al, %eax
	addl	%eax, -32(%rbp)
	movl	-40(%rbp), %edx
	movl	-20(%rbp), %eax
	addl	%edx, %eax
	andl	%eax, -40(%rbp)
	cmpl	$0, -40(%rbp)
	setne	%al
	movzbl	%al, %eax
	addl	%eax, -32(%rbp)
	movl	-40(%rbp), %edx
	movl	-20(%rbp), %eax
	addl	%edx, %eax
	andl	%eax, -40(%rbp)
	cmpl	$0, -40(%rbp)
	setne	%al
	movzbl	%al, %eax
	addl	%eax, -32(%rbp)
	movl	-40(%rbp), %edx
	movl	-20(%rbp), %eax
	addl	%edx, %eax
	andl	%eax, -40(%rbp)
	cmpl	$0, -40(%rbp)
	setne	%al
	movzbl	%al, %eax
	addl	%eax, -32(%rbp)
	movl	-40(%rbp), %edx
	movl	-20(%rbp), %eax
	addl	%edx, %eax
	andl	%eax, -40(%rbp)
	cmpl	$0, -40(%rbp)
	setne	%al
	movzbl	%al, %eax
	addl	%eax, -32(%rbp)
	movl	-40(%rbp), %edx
	movl	-20(%rbp), %eax
	addl	%edx, %eax
	andl	%eax, -40(%rbp)
	cmpl	$0, -40(%rbp)
	setne	%al
	movzbl	%al, %eax
	addl	%eax, -32(%rbp)
	movl	-40(%rbp), %edx
	movl	-20(%rbp), %eax
	addl	%edx, %eax
	andl	%eax, -40(%rbp)
	cmpl	$0, -40(%rbp)
	setne	%al
	movzbl	%al, %eax
	addl	%eax, -32(%rbp)
	movl	-40(%rbp), %edx
	movl	-20(%rbp), %eax
	addl	%edx, %eax
	andl	%eax, -40(%rbp)
	cmpl	$0, -40(%rbp)
	setne	%al
	movzbl	%al, %eax
	addl	%eax, -32(%rbp)
	cmpl	$0, -36(%rbp)
	setne	%al
	movzbl	%al, %eax
	addl	%eax, -28(%rbp)
	movl	-36(%rbp), %edx
	movl	-20(%rbp), %eax
	addl	%edx, %eax
	andl	%eax, -36(%rbp)
	cmpl	$0, -36(%rbp)
	setne	%al
	movzbl	%al, %eax
	addl	%eax, -28(%rbp)
	movl	-36(%rbp), %edx
	movl	-20(%rbp), %eax
	addl	%edx, %eax
	andl	%eax, -36(%rbp)
	cmpl	$0, -36(%rbp)
	setne	%al
	movzbl	%al, %eax
	addl	%eax, -28(%rbp)
	movl	-36(%rbp), %edx
	movl	-20(%rbp), %eax
	addl	%edx, %eax
	andl	%eax, -36(%rbp)
	cmpl	$0, -36(%rbp)
	setne	%al
	movzbl	%al, %eax
	addl	%eax, -28(%rbp)
	movl	-36(%rbp), %edx
	movl	-20(%rbp), %eax
	addl	%edx, %eax
	andl	%eax, -36(%rbp)
	cmpl	$0, -36(%rbp)
	setne	%al
	movzbl	%al, %eax
	addl	%eax, -28(%rbp)
	movl	-36(%rbp), %edx
	movl	-20(%rbp), %eax
	addl	%edx, %eax
	andl	%eax, -36(%rbp)
	cmpl	$0, -36(%rbp)
	setne	%al
	movzbl	%al, %eax
	addl	%eax, -28(%rbp)
	movl	-36(%rbp), %edx
	movl	-20(%rbp), %eax
	addl	%edx, %eax
	andl	%eax, -36(%rbp)
	cmpl	$0, -36(%rbp)
	setne	%al
	movzbl	%al, %eax
	addl	%eax, -28(%rbp)
	movl	-36(%rbp), %edx
	movl	-20(%rbp), %eax
	addl	%edx, %eax
	andl	%eax, -36(%rbp)
	cmpl	$0, -36(%rbp)
	setne	%al
	movzbl	%al, %eax
	addl	%eax, -28(%rbp)
	movl	-36(%rbp), %edx
	movl	-20(%rbp), %eax
	addl	%edx, %eax
	andl	%eax, -36(%rbp)
	cmpl	$0, -36(%rbp)
	setne	%al
	movzbl	%al, %eax
	addl	%eax, -28(%rbp)
	movl	-36(%rbp), %edx
	movl	-20(%rbp), %eax
	addl	%edx, %eax
	andl	%eax, -36(%rbp)
	cmpl	$0, -36(%rbp)
	setne	%al
	movzbl	%al, %eax
	addl	%eax, -28(%rbp)
	movl	-36(%rbp), %edx
	movl	-20(%rbp), %eax
	addl	%edx, %eax
	andl	%eax, -36(%rbp)
	cmpl	$0, -36(%rbp)
	setne	%al
	movzbl	%al, %eax
	addl	%eax, -28(%rbp)
	movl	-36(%rbp), %edx
	movl	-20(%rbp), %eax
	addl	%edx, %eax
	andl	%eax, -36(%rbp)
	cmpl	$0, -36(%rbp)
	setne	%al
	movzbl	%al, %eax
	addl	%eax, -28(%rbp)
	movl	-36(%rbp), %edx
	movl	-20(%rbp), %eax
	addl	%edx, %eax
	andl	%eax, -36(%rbp)
	cmpl	$0, -36(%rbp)
	setne	%al
	movzbl	%al, %eax
	addl	%eax, -28(%rbp)
	movl	-36(%rbp), %edx
	movl	-20(%rbp), %eax
	addl	%edx, %eax
	andl	%eax, -36(%rbp)
	cmpl	$0, -36(%rbp)
	setne	%al
	movzbl	%al, %eax
	addl	%eax, -28(%rbp)
	movl	-36(%rbp), %edx
	movl	-20(%rbp), %eax
	addl	%edx, %eax
	andl	%eax, -36(%rbp)
	cmpl	$0, -36(%rbp)
	setne	%al
	movzbl	%al, %eax
	addl	%eax, -28(%rbp)
	movl	-36(%rbp), %edx
	movl	-20(%rbp), %eax
	addl	%edx, %eax
	andl	%eax, -36(%rbp)
	cmpl	$0, -36(%rbp)
	setne	%al
	movzbl	%al, %eax
	addl	%eax, -28(%rbp)
	movl	-36(%rbp), %edx
	movl	-20(%rbp), %eax
	addl	%edx, %eax
	andl	%eax, -36(%rbp)
	cmpl	$0, -36(%rbp)
	setne	%al
	movzbl	%al, %eax
	addl	%eax, -28(%rbp)
	movl	-28(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-32(%rbp), %eax
	addl	%edx, %eax
	movl	%eax, -24(%rbp)
	movl	$1, -12(%rbp)
	movl	$0, -8(%rbp)
	movl	$4, -4(%rbp)
	movl	-4(%rbp), %eax
	andl	-52(%rbp), %eax
	testl	%eax, %eax
	jne	.L22
	movl	$0, %eax
	jmp	.L23
.L22:
	movl	$-1, %eax
.L23:
	andl	-12(%rbp), %eax
	addl	%eax, -8(%rbp)
	sall	-4(%rbp)
	movl	-12(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-4(%rbp), %eax
	andl	-48(%rbp), %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %eax
	andl	-52(%rbp), %eax
	testl	%eax, %eax
	jne	.L24
	movl	$0, %eax
	jmp	.L25
.L24:
	movl	$-1, %eax
.L25:
	andl	-12(%rbp), %eax
	addl	%eax, -8(%rbp)
	sall	-4(%rbp)
	movl	-12(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-4(%rbp), %eax
	andl	-48(%rbp), %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %eax
	andl	-52(%rbp), %eax
	testl	%eax, %eax
	jne	.L26
	movl	$0, %eax
	jmp	.L27
.L26:
	movl	$-1, %eax
.L27:
	andl	-12(%rbp), %eax
	addl	%eax, -8(%rbp)
	sall	-4(%rbp)
	movl	-12(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-4(%rbp), %eax
	andl	-48(%rbp), %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %eax
	andl	-52(%rbp), %eax
	testl	%eax, %eax
	jne	.L28
	movl	$0, %eax
	jmp	.L29
.L28:
	movl	$-1, %eax
.L29:
	andl	-12(%rbp), %eax
	addl	%eax, -8(%rbp)
	sall	-4(%rbp)
	movl	-12(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-4(%rbp), %eax
	andl	-48(%rbp), %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %eax
	andl	-52(%rbp), %eax
	testl	%eax, %eax
	jne	.L30
	movl	$0, %eax
	jmp	.L31
.L30:
	movl	$-1, %eax
.L31:
	andl	-12(%rbp), %eax
	addl	%eax, -8(%rbp)
	sall	-4(%rbp)
	movl	-12(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-4(%rbp), %eax
	andl	-48(%rbp), %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %eax
	andl	-52(%rbp), %eax
	testl	%eax, %eax
	jne	.L32
	movl	$0, %eax
	jmp	.L33
.L32:
	movl	$-1, %eax
.L33:
	andl	-12(%rbp), %eax
	addl	%eax, -8(%rbp)
	sall	-4(%rbp)
	movl	-12(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-4(%rbp), %eax
	andl	-48(%rbp), %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %eax
	andl	-52(%rbp), %eax
	testl	%eax, %eax
	jne	.L34
	movl	$0, %eax
	jmp	.L35
.L34:
	movl	$-1, %eax
.L35:
	andl	-12(%rbp), %eax
	addl	%eax, -8(%rbp)
	sall	-4(%rbp)
	movl	-12(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-4(%rbp), %eax
	andl	-48(%rbp), %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %eax
	andl	-52(%rbp), %eax
	testl	%eax, %eax
	jne	.L36
	movl	$0, %eax
	jmp	.L37
.L36:
	movl	$-1, %eax
.L37:
	andl	-12(%rbp), %eax
	addl	%eax, -8(%rbp)
	sall	-4(%rbp)
	movl	-12(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-4(%rbp), %eax
	andl	-48(%rbp), %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %eax
	andl	-52(%rbp), %eax
	testl	%eax, %eax
	jne	.L38
	movl	$0, %eax
	jmp	.L39
.L38:
	movl	$-1, %eax
.L39:
	andl	-12(%rbp), %eax
	addl	%eax, -8(%rbp)
	sall	-4(%rbp)
	movl	-12(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-4(%rbp), %eax
	andl	-48(%rbp), %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %eax
	andl	-52(%rbp), %eax
	testl	%eax, %eax
	jne	.L40
	movl	$0, %eax
	jmp	.L41
.L40:
	movl	$-1, %eax
.L41:
	andl	-12(%rbp), %eax
	addl	%eax, -8(%rbp)
	sall	-4(%rbp)
	movl	-12(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-4(%rbp), %eax
	andl	-48(%rbp), %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %eax
	andl	-52(%rbp), %eax
	testl	%eax, %eax
	jne	.L42
	movl	$0, %eax
	jmp	.L43
.L42:
	movl	$-1, %eax
.L43:
	andl	-12(%rbp), %eax
	addl	%eax, -8(%rbp)
	sall	-4(%rbp)
	movl	-12(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-4(%rbp), %eax
	andl	-48(%rbp), %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %eax
	andl	-52(%rbp), %eax
	testl	%eax, %eax
	jne	.L44
	movl	$0, %eax
	jmp	.L45
.L44:
	movl	$-1, %eax
.L45:
	andl	-12(%rbp), %eax
	addl	%eax, -8(%rbp)
	sall	-4(%rbp)
	movl	-12(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-4(%rbp), %eax
	andl	-48(%rbp), %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %eax
	andl	-52(%rbp), %eax
	testl	%eax, %eax
	jne	.L46
	movl	$0, %eax
	jmp	.L47
.L46:
	movl	$-1, %eax
.L47:
	andl	-12(%rbp), %eax
	addl	%eax, -8(%rbp)
	sall	-4(%rbp)
	movl	-12(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-4(%rbp), %eax
	andl	-48(%rbp), %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %eax
	andl	-52(%rbp), %eax
	testl	%eax, %eax
	jne	.L48
	movl	$0, %eax
	jmp	.L49
.L48:
	movl	$-1, %eax
.L49:
	andl	-12(%rbp), %eax
	addl	%eax, -8(%rbp)
	sall	-4(%rbp)
	movl	-12(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-4(%rbp), %eax
	andl	-48(%rbp), %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %eax
	andl	-52(%rbp), %eax
	testl	%eax, %eax
	jne	.L50
	movl	$0, %eax
	jmp	.L51
.L50:
	movl	$-1, %eax
.L51:
	andl	-12(%rbp), %eax
	addl	%eax, -8(%rbp)
	sall	-4(%rbp)
	movl	-12(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-4(%rbp), %eax
	andl	-48(%rbp), %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %eax
	andl	-52(%rbp), %eax
	testl	%eax, %eax
	jne	.L52
	movl	$0, %eax
	jmp	.L53
.L52:
	movl	$-1, %eax
.L53:
	andl	-12(%rbp), %eax
	addl	%eax, -8(%rbp)
	sall	-4(%rbp)
	movl	-12(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-4(%rbp), %eax
	andl	-48(%rbp), %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %eax
	andl	-52(%rbp), %eax
	testl	%eax, %eax
	jne	.L54
	movl	$0, %eax
	jmp	.L55
.L54:
	movl	$-1, %eax
.L55:
	andl	-12(%rbp), %eax
	addl	%eax, -8(%rbp)
	sall	-4(%rbp)
	movl	-12(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-4(%rbp), %eax
	andl	-48(%rbp), %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %eax
	andl	-52(%rbp), %eax
	testl	%eax, %eax
	jne	.L56
	movl	$0, %eax
	jmp	.L57
.L56:
	movl	$-1, %eax
.L57:
	andl	-12(%rbp), %eax
	addl	%eax, -8(%rbp)
	sall	-4(%rbp)
	movl	-12(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-4(%rbp), %eax
	andl	-48(%rbp), %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %eax
	andl	-52(%rbp), %eax
	testl	%eax, %eax
	jne	.L58
	movl	$0, %eax
	jmp	.L59
.L58:
	movl	$-1, %eax
.L59:
	andl	-12(%rbp), %eax
	addl	%eax, -8(%rbp)
	sall	-4(%rbp)
	movl	-12(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-4(%rbp), %eax
	andl	-48(%rbp), %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %eax
	andl	-52(%rbp), %eax
	testl	%eax, %eax
	jne	.L60
	movl	$0, %eax
	jmp	.L61
.L60:
	movl	$-1, %eax
.L61:
	andl	-12(%rbp), %eax
	addl	%eax, -8(%rbp)
	sall	-4(%rbp)
	movl	-12(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-4(%rbp), %eax
	andl	-48(%rbp), %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %eax
	andl	-52(%rbp), %eax
	testl	%eax, %eax
	jne	.L62
	movl	$0, %eax
	jmp	.L63
.L62:
	movl	$-1, %eax
.L63:
	andl	-12(%rbp), %eax
	addl	%eax, -8(%rbp)
	sall	-4(%rbp)
	movl	-12(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-4(%rbp), %eax
	andl	-48(%rbp), %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %eax
	andl	-52(%rbp), %eax
	testl	%eax, %eax
	jne	.L64
	movl	$0, %eax
	jmp	.L65
.L64:
	movl	$-1, %eax
.L65:
	andl	-12(%rbp), %eax
	addl	%eax, -8(%rbp)
	sall	-4(%rbp)
	movl	-12(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-4(%rbp), %eax
	andl	-48(%rbp), %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %eax
	andl	-52(%rbp), %eax
	testl	%eax, %eax
	jne	.L66
	movl	$0, %eax
	jmp	.L67
.L66:
	movl	$-1, %eax
.L67:
	andl	-12(%rbp), %eax
	addl	%eax, -8(%rbp)
	sall	-4(%rbp)
	movl	-12(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-4(%rbp), %eax
	andl	-48(%rbp), %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %eax
	andl	-52(%rbp), %eax
	testl	%eax, %eax
	jne	.L68
	movl	$0, %eax
	jmp	.L69
.L68:
	movl	$-1, %eax
.L69:
	andl	-12(%rbp), %eax
	addl	%eax, -8(%rbp)
	sall	-4(%rbp)
	movl	-12(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-4(%rbp), %eax
	andl	-48(%rbp), %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %eax
	andl	-52(%rbp), %eax
	testl	%eax, %eax
	jne	.L70
	movl	$0, %eax
	jmp	.L71
.L70:
	movl	$-1, %eax
.L71:
	andl	-12(%rbp), %eax
	addl	%eax, -8(%rbp)
	sall	-4(%rbp)
	movl	-12(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-4(%rbp), %eax
	andl	-48(%rbp), %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %eax
	andl	-52(%rbp), %eax
	testl	%eax, %eax
	jne	.L72
	movl	$0, %eax
	jmp	.L73
.L72:
	movl	$-1, %eax
.L73:
	andl	-12(%rbp), %eax
	addl	%eax, -8(%rbp)
	sall	-4(%rbp)
	movl	-12(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-4(%rbp), %eax
	andl	-48(%rbp), %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %eax
	andl	-52(%rbp), %eax
	testl	%eax, %eax
	jne	.L74
	movl	$0, %eax
	jmp	.L75
.L74:
	movl	$-1, %eax
.L75:
	andl	-12(%rbp), %eax
	addl	%eax, -8(%rbp)
	sall	-4(%rbp)
	movl	-12(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-4(%rbp), %eax
	andl	-48(%rbp), %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %eax
	andl	-52(%rbp), %eax
	testl	%eax, %eax
	jne	.L76
	movl	$0, %eax
	jmp	.L77
.L76:
	movl	$-1, %eax
.L77:
	andl	-12(%rbp), %eax
	addl	%eax, -8(%rbp)
	sall	-4(%rbp)
	movl	-12(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-4(%rbp), %eax
	andl	-48(%rbp), %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %eax
	andl	-52(%rbp), %eax
	testl	%eax, %eax
	jne	.L78
	movl	$0, %eax
	jmp	.L79
.L78:
	movl	$-1, %eax
.L79:
	andl	-12(%rbp), %eax
	addl	%eax, -8(%rbp)
	movl	-8(%rbp), %eax
	addl	%eax, -56(%rbp)
	movl	$1, -12(%rbp)
	movl	$0, -8(%rbp)
	movl	$4, -4(%rbp)
	movl	-4(%rbp), %eax
	andl	-24(%rbp), %eax
	testl	%eax, %eax
	jne	.L80
	movl	$0, %eax
	jmp	.L81
.L80:
	movl	$-1, %eax
.L81:
	andl	-12(%rbp), %eax
	addl	%eax, -8(%rbp)
	sall	-4(%rbp)
	movl	-12(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-4(%rbp), %eax
	andl	-48(%rbp), %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %eax
	andl	-24(%rbp), %eax
	testl	%eax, %eax
	jne	.L82
	movl	$0, %eax
	jmp	.L83
.L82:
	movl	$-1, %eax
.L83:
	andl	-12(%rbp), %eax
	addl	%eax, -8(%rbp)
	sall	-4(%rbp)
	movl	-12(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-4(%rbp), %eax
	andl	-48(%rbp), %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %eax
	andl	-24(%rbp), %eax
	testl	%eax, %eax
	jne	.L84
	movl	$0, %eax
	jmp	.L85
.L84:
	movl	$-1, %eax
.L85:
	andl	-12(%rbp), %eax
	addl	%eax, -8(%rbp)
	sall	-4(%rbp)
	movl	-12(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-4(%rbp), %eax
	andl	-48(%rbp), %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %eax
	andl	-24(%rbp), %eax
	testl	%eax, %eax
	jne	.L86
	movl	$0, %eax
	jmp	.L87
.L86:
	movl	$-1, %eax
.L87:
	andl	-12(%rbp), %eax
	addl	%eax, -8(%rbp)
	sall	-4(%rbp)
	movl	-12(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-4(%rbp), %eax
	andl	-48(%rbp), %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %eax
	andl	-24(%rbp), %eax
	testl	%eax, %eax
	jne	.L88
	movl	$0, %eax
	jmp	.L89
.L88:
	movl	$-1, %eax
.L89:
	andl	-12(%rbp), %eax
	addl	%eax, -8(%rbp)
	sall	-4(%rbp)
	movl	-12(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-4(%rbp), %eax
	andl	-48(%rbp), %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %eax
	andl	-24(%rbp), %eax
	testl	%eax, %eax
	jne	.L90
	movl	$0, %eax
	jmp	.L91
.L90:
	movl	$-1, %eax
.L91:
	andl	-12(%rbp), %eax
	addl	%eax, -8(%rbp)
	movl	-8(%rbp), %eax
	addl	%eax, -56(%rbp)
	movl	-24(%rbp), %eax
	andl	$64, %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %edx
	movl	-24(%rbp), %eax
	andl	$16, %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%eax, %edx
	movl	-24(%rbp), %eax
	andl	$4, %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%eax, %edx
	movl	-24(%rbp), %eax
	andl	$1, %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, -32(%rbp)
	movl	-24(%rbp), %eax
	andl	$128, %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %edx
	movl	-24(%rbp), %eax
	andl	$32, %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%eax, %edx
	movl	-24(%rbp), %eax
	andl	$2, %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%eax, %edx
	movl	-24(%rbp), %eax
	andl	$8, %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, -28(%rbp)
	movl	-28(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-32(%rbp), %eax
	addl	%edx, %eax
	movl	%eax, -24(%rbp)
	movl	$1, -12(%rbp)
	movl	$0, -8(%rbp)
	movl	$4, -4(%rbp)
	movl	-4(%rbp), %eax
	andl	-24(%rbp), %eax
	testl	%eax, %eax
	jne	.L92
	movl	$0, %eax
	jmp	.L93
.L92:
	movl	$-1, %eax
.L93:
	andl	-12(%rbp), %eax
	addl	%eax, -8(%rbp)
	sall	-4(%rbp)
	movl	-12(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-4(%rbp), %eax
	andl	-48(%rbp), %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %eax
	andl	-24(%rbp), %eax
	testl	%eax, %eax
	jne	.L94
	movl	$0, %eax
	jmp	.L95
.L94:
	movl	$-1, %eax
.L95:
	andl	-12(%rbp), %eax
	addl	%eax, -8(%rbp)
	movl	-8(%rbp), %eax
	addl	%eax, -56(%rbp)
	movl	-24(%rbp), %eax
	andl	$1, %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %edx
	movl	-24(%rbp), %eax
	andl	$4, %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, -32(%rbp)
	movl	-24(%rbp), %eax
	andl	$2, %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %edx
	movl	-24(%rbp), %eax
	andl	$8, %eax
	testl	%eax, %eax
	setne	%al
	movzbl	%al, %eax
	addl	%edx, %eax
	movl	%eax, -28(%rbp)
	movl	-28(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-32(%rbp), %eax
	addl	%edx, %eax
	movl	%eax, -24(%rbp)
	movl	$1, -12(%rbp)
	movl	$0, -8(%rbp)
	movl	$4, -4(%rbp)
	movl	-4(%rbp), %eax
	andl	-24(%rbp), %eax
	testl	%eax, %eax
	jne	.L96
	movl	$0, %eax
	jmp	.L97
.L96:
	movl	$-1, %eax
.L97:
	andl	-12(%rbp), %eax
	addl	%eax, -8(%rbp)
	movl	-8(%rbp), %eax
	addl	%eax, -56(%rbp)
	cmpl	$3, -24(%rbp)
	sete	%al
	movzbl	%al, %eax
	addl	%eax, -56(%rbp)
	sarl	-56(%rbp)
	movl	-56(%rbp), %eax
	xorl	-60(%rbp), %eax
	movl	%eax, %edx
	cmpl	$0, -60(%rbp)
	setne	%al
	movzbl	%al, %eax
	addl	%eax, %edx
	movl	-64(%rbp), %eax
	sall	$4, %eax
	movl	%eax, %ecx
	sall	%cl, %edx
	movl	-64(%rbp), %eax
	sall	$4, %eax
	movl	%eax, %ecx
	sall	%cl, %edx
	cmpl	$0, -64(%rbp)
	jne	.L98
	movl	$16, %ecx
	jmp	.L99
.L98:
	movl	$0, %ecx
.L99:
	movl	-16(%rbp), %eax
	sall	%cl, %eax
	movl	%eax, %esi
	cmpl	$0, -64(%rbp)
	jne	.L100
	movl	$16, %eax
	jmp	.L101
.L100:
	movl	$0, %eax
.L101:
	movl	%eax, %ecx
	sall	%cl, %esi
	movl	%esi, %eax
	orl	%edx, %eax
	movl	%eax, -56(%rbp)
	movl	-56(%rbp), %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE10:
	.size	divSix, .-divSix
	.globl	modThree
	.type	modThree, @function
modThree:
.LFB11:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -36(%rbp)
	movl	-36(%rbp), %eax
	sarl	$31, %eax
	movl	%eax, -32(%rbp)
	movl	-32(%rbp), %eax
	andl	$1, %eax
	movl	%eax, -28(%rbp)
	movl	-36(%rbp), %eax
	xorl	-32(%rbp), %eax
	movl	%eax, %edx
	movl	-28(%rbp), %eax
	addl	%edx, %eax
	movl	%eax, -24(%rbp)
	movl	$771, -20(%rbp)
	movl	-20(%rbp), %eax
	sall	$16, %eax
	orl	%eax, -20(%rbp)
	movl	-24(%rbp), %eax
	andl	-20(%rbp), %eax
	movl	%eax, -16(%rbp)
	movl	-24(%rbp), %eax
	sarl	$2, %eax
	andl	-20(%rbp), %eax
	addl	%eax, -16(%rbp)
	movl	-24(%rbp), %eax
	sarl	$4, %eax
	andl	-20(%rbp), %eax
	addl	%eax, -16(%rbp)
	movl	-24(%rbp), %eax
	sarl	$6, %eax
	andl	-20(%rbp), %eax
	addl	%eax, -16(%rbp)
	movl	-16(%rbp), %eax
	sarl	$16, %eax
	addl	%eax, -16(%rbp)
	movl	-16(%rbp), %eax
	sarl	$8, %eax
	addl	%eax, -16(%rbp)
	movl	-16(%rbp), %eax
	andl	$51, %eax
	movl	%eax, -12(%rbp)
	movl	-16(%rbp), %eax
	sarl	$2, %eax
	andl	$51, %eax
	addl	%eax, -12(%rbp)
	movl	-12(%rbp), %eax
	sarl	$4, %eax
	addl	%eax, -12(%rbp)
	movl	-12(%rbp), %eax
	andl	$3, %eax
	movl	%eax, %edx
	movl	-12(%rbp), %eax
	sarl	$2, %eax
	andl	$3, %eax
	addl	%edx, %eax
	movl	%eax, -8(%rbp)
	movl	-8(%rbp), %eax
	andl	$3, %eax
	movl	%eax, %edx
	movl	-8(%rbp), %eax
	sarl	$2, %eax
	andl	$1, %eax
	addl	%edx, %eax
	movl	%eax, -4(%rbp)
	andl	$3, -4(%rbp)
	cmpl	$3, -4(%rbp)
	jne	.L104
	movl	$2, %eax
	jmp	.L105
.L104:
	movl	$0, %eax
.L105:
	movl	%eax, %ecx
	sarl	%cl, -4(%rbp)
	movl	-32(%rbp), %eax
	xorl	-4(%rbp), %eax
	movl	%eax, %edx
	movl	-28(%rbp), %eax
	addl	%edx, %eax
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE11:
	.size	modThree, .-modThree
	.globl	float_f2i
	.type	float_f2i, @function
float_f2i:
.LFB12:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -52(%rbp)
	movl	$-1, -36(%rbp)
	movl	$-2147483648, -32(%rbp)
	movl	-36(%rbp), %eax
	sall	$7, %eax
	addl	$1, %eax
	movl	%eax, -28(%rbp)
	movl	-36(%rbp), %eax
	addl	$256, %eax
	movl	%eax, -24(%rbp)
	movl	-52(%rbp), %eax
	shrl	$23, %eax
	movl	%eax, %edx
	movl	-24(%rbp), %eax
	andl	%edx, %eax
	movl	%eax, -20(%rbp)
	movl	-20(%rbp), %edx
	movl	-28(%rbp), %eax
	addl	%edx, %eax
	movl	%eax, -16(%rbp)
	movl	-16(%rbp), %eax
	negl	%eax
	movl	%eax, -12(%rbp)
	movl	-12(%rbp), %eax
	addl	$31, %eax
	movl	%eax, -8(%rbp)
	movl	-52(%rbp), %eax
	sall	$8, %eax
	movl	%eax, %edx
	movl	-32(%rbp), %eax
	orl	%edx, %eax
	movl	%eax, -4(%rbp)
	movl	-8(%rbp), %eax
	movl	-4(%rbp), %edx
	movl	%eax, %ecx
	shrl	%cl, %edx
	movl	%edx, %eax
	movl	%eax, -40(%rbp)
	movl	-52(%rbp), %eax
	testl	%eax, %eax
	jns	.L108
	negl	-40(%rbp)
.L108:
	movl	-8(%rbp), %edx
	movl	-36(%rbp), %eax
	addl	%edx, %eax
	testl	%eax, %eax
	jns	.L109
	movl	-32(%rbp), %eax
	jmp	.L110
.L109:
	cmpl	$0, -16(%rbp)
	jns	.L111
	movl	$0, %eax
	jmp	.L110
.L111:
	movl	-20(%rbp), %eax
	cmpl	-24(%rbp), %eax
	jne	.L112
	movl	-32(%rbp), %eax
	jmp	.L110
.L112:
	movl	-40(%rbp), %eax
.L110:
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE12:
	.size	float_f2i, .-float_f2i
	.globl	float_half
	.type	float_half, @function
float_half:
.LFB13:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -52(%rbp)
	movl	$-2147483648, -36(%rbp)
	movl	$-1, -32(%rbp)
	movl	-32(%rbp), %eax
	shrl	$24, %eax
	movl	%eax, -28(%rbp)
	movl	-52(%rbp), %eax
	shrl	$23, %eax
	movl	%eax, %edx
	movl	-28(%rbp), %eax
	andl	%edx, %eax
	movl	%eax, -24(%rbp)
	movl	-24(%rbp), %edx
	movl	-32(%rbp), %eax
	addl	%edx, %eax
	movl	%eax, -20(%rbp)
	movl	-28(%rbp), %eax
	sall	$23, %eax
	notl	%eax
	andl	-52(%rbp), %eax
	movl	%eax, -16(%rbp)
	movl	-20(%rbp), %eax
	sall	$23, %eax
	orl	-16(%rbp), %eax
	movl	%eax, -40(%rbp)
	movl	-24(%rbp), %eax
	cmpl	-28(%rbp), %eax
	jne	.L114
	movl	-52(%rbp), %eax
	jmp	.L115
.L114:
	movl	-32(%rbp), %eax
	leal	(%rax,%rax), %edx
	movl	-24(%rbp), %eax
	andl	%edx, %eax
	testl	%eax, %eax
	jne	.L116
	movl	-36(%rbp), %eax
	andl	-52(%rbp), %eax
	movl	%eax, -12(%rbp)
	movl	-32(%rbp), %eax
	shrl	%eax
	andl	-52(%rbp), %eax
	movl	%eax, -8(%rbp)
	movl	-8(%rbp), %eax
	andl	$1, %eax
	testl	%eax, %eax
	je	.L117
	movl	-8(%rbp), %eax
	andl	$2, %eax
	testl	%eax, %eax
	je	.L117
	movl	$1, %eax
	jmp	.L118
.L117:
	movl	$0, %eax
.L118:
	movl	%eax, -4(%rbp)
	sarl	-8(%rbp)
	movl	-12(%rbp), %eax
	orl	-8(%rbp), %eax
	movl	%eax, -40(%rbp)
	cmpl	$0, -8(%rbp)
	je	.L119
	movl	-4(%rbp), %eax
	addl	%eax, -40(%rbp)
.L119:
	movl	-40(%rbp), %eax
	jmp	.L115
.L116:
	movl	-40(%rbp), %eax
.L115:
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE13:
	.size	float_half, .-float_half
	.globl	float_construct
	.type	float_construct, @function
float_construct:
.LFB14:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	$2, %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE14:
	.size	float_construct, .-float_construct
	.ident	"GCC: (Ubuntu 7.3.0-16ubuntu3) 7.3.0"
	.section	.note.GNU-stack,"",@progbits
