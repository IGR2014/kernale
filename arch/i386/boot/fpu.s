################################################################
#
#	FPU operations
#
#	File:	fpu.s
#	Date:	05 Jul 2021
#
#	Copyright (c) 2017 - 2021, Igor Baklykov
#	All rights reserved.
#
#


.set	FPU_STATUS_WORD_TEST,	0xDEAD		# FPU status word test
.set	FPU_CONTROL_WORD_MASK,	0x103F		# FPU control word mask
.set	FPU_CONTROL_WORD,	0x003F		# FPU control word


.code32

.section .text
.balign 4
.global fpuCheck				# Check FPU


# Check FPU
.type fpuCheck, @function
fpuCheck:
	fninit					# Initialize FPU
	fnstsw $FPU_STATUS_WORD_TEST		# Store FPU status word
	cmpl %eax, 0				# Compare FPU status word with 0
	jnel 1f					# Not equal - no FPU
	fnstcw %ax				# Store FPU control word
	andw $FPU_CONTROL_WORD_MASK, %ax	# Apply mask to FPU control word
	cmpl $FPU_CONTROL_WORD, %ax		# Compare FPU control word with 0
	jnel 1f					# Not equal - no FPU
	movl $1, %eax				# FPU - ok
	retl					# Exit
1:
	movl $0, $eax				# FPU - error
	retl					# Exit
.size fpuCheck, . - fpuCheck