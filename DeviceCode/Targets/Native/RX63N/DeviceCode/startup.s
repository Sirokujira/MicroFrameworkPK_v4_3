############################################################################
## Copyright (c) 2010-2011 Kentaro Sekimoto  All rights reserved.
############################################################################

    .section    .text, "ax", %progbits

    .global _EntryPoint
    .global _startup
    .global _HARD_Breakpoint
    .global _StackBottom
    .global _StackTop
    .global _HeapBegin
    .global _HeapEnd
    .global _CustomHeapBegin
    .global _CustomHeapEnd

	.extern _BootstrapCode
    .extern _RelocatableVectors

    .section SectionForStackBottom,     "a", %progbits
_StackBottom: .word 0
    .section SectionForStackTop,        "a", %progbits
_StackTop:    .word 0
    .section SectionForHeapBegin,       "a", %progbits
_HeapBegin:   .word 0
    .section SectionForHeapEnd,         "a", %progbits
_HeapEnd:     .word 0
    .section SectionForCustomHeapBegin, "a", %progbits
_CustomHeapBegin:   .word 0
    .section SectionForCustomHeapEnd,   "a", %progbits
_CustomHeapEnd:     .word 0

    .section i.EntryPoint, "ax", %progbits

    .org    0x000
_EntryPoint:
_startup:
    nop
    nop
    nop
    nop
/* initialise user stack pointer */
    mvtc    #_StackTop-4,USP
/* initialise interrupt stack pointer */
    mvtc    #_StackTop-4,ISP
/* setup intb */
    mvtc    #_RelocatableVectors,intb    /* INTERRUPT VECTOR ADDRESS  definition    */
/* setup FPSW */
    mvtc    #100h, fpsw
/* setup PSW */
    /*mvtc    #10000h, psw */           /* Set Ubit & Ibit for PSW */
    mvtc    #00000h, psw            /* Clear Ubit & Ibit for PSW */

/* start user program */
	bsr.a	_BootstrapCode
    bsr.a   _BootEntry
/* call to exit*/
loop_here:
    bra loop_here
    
_HARD_Breakpoint:
	bra	_HARD_Breakpoint

    .text
    .end
