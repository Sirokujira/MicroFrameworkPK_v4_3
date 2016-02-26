;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
;@ Copyright (c) Microsoft Corporation.  All rights reserved.
;@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

     .align 4
     .section   tinyclr_metadata, "a", %progbits

    .global  _TinyClr_Dat_Start
    .global  _TinyClr_Dat_End

_TinyClr_Dat_Start:
    .incbin "tinyclr.dat"
_TinyClr_Dat_End:

    .end

