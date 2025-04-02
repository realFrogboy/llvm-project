	.file	"fib.cpp"
	.text
	.globl	_Z3fibi                         ; -- Begin function _Z3fibi
	.type	_Z3fibi,@function
_Z3fibi:                                ; @_Z3fibi
; %bb.0:
	addi	x2, x2, -24
	sd	x1, 16(x2)                      ; 8-byte Folded Spill
	sd	x8, 8(x2)                       ; 8-byte Folded Spill
	addi	x8, x2, 24
	sw	x10, -24(x8)
	lwu	x10, -24(x8)
	addi	x11, x0, 1
	bne	x10, x11, .LBB0_2
	jal	x0, .LBB0_1
.LBB0_1:
	sw	x0, -20(x8)
	jal	x0, .LBB0_5
.LBB0_2:
	lwu	x10, -24(x8)
	addi	x11, x0, 2
	bne	x10, x11, .LBB0_4
	jal	x0, .LBB0_3
.LBB0_3:
	addi	x10, x0, 1
	sw	x10, -20(x8)
	jal	x0, .LBB0_5
.LBB0_4:
	lw	x10, -24(x8)
	addi	x10, x10, -2
	call	_Z3fibi
	addi	x8, x10, 0
	lw	x10, -24(x8)
	addi	x10, x10, -1
	call	_Z3fibi
	add	x10, x8, x10
	sw	x10, -20(x8)
	jal	x0, .LBB0_5
.LBB0_5:
	lw	x10, -20(x8)
	ld	x8, 8(x2)                       ; 8-byte Folded Reload
	ld	x1, 16(x2)                      ; 8-byte Folded Reload
	addi	x2, x2, 24
	jalr	x0, 0(x1)
.Lfunc_end0:
	.size	_Z3fibi, .Lfunc_end0-_Z3fibi
                                        ; -- End function
	.globl	main                            ; -- Begin function main
	.type	main,@function
main:                                   ; @main
; %bb.0:
	addi	x2, x2, -20
	sd	x1, 12(x2)                      ; 8-byte Folded Spill
	sd	x8, 4(x2)                       ; 8-byte Folded Spill
	addi	x8, x2, 20
	sw	x0, -20(x8)
	addi	x10, x0, 5
	call	_Z3fibi
	ld	x8, 4(x2)                       ; 8-byte Folded Reload
	ld	x1, 12(x2)                      ; 8-byte Folded Reload
	addi	x2, x2, 20
	jalr	x0, 0(x1)
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
                                        ; -- End function
	.ident	"clang version 21.0.0git (git@github.com:realFrogboy/llvm-project.git b02a22e7f3b72afa57da55c297acce9e811f62d4)"
	.section	".note.GNU-stack","",@progbits
