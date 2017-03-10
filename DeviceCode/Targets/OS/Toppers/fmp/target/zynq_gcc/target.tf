$
$  Valid local interrupts: SGI(0-0xF = 0-16) and PPI(0x1B-0x1F = 27-31)
$  Valid global interrupts: SPI(0x20-0x5F = 32-95)
$
$INTNO_VALID = {
	0x10000,0x10001,...,0x1000f;0x1001b,0x1001c,...,0x1001f;
	0x20000,0x20001,...,0x2000f;0x2001b,0x2001c,...,0x2001f;
	0x00020,0x00021,...,0x0005f
}$

$
$  Valid interrupt handlers (global + private):
$  SGI(0-0xF = 0-16) + PPI(0x1B-0x1F = 27-31) + SPI(0x20-0x5F = 32-95)
$
$INHNO_VALID = {
	0x10000,0x10001,...,0x1000f;0x1001b,0x1001c,...,0x1005f;
	0x20000,0x20001,...,0x2000f;0x2001b,0x2001c,...,0x2005f
}$

$
$  Interrupt numbers usable in ATT_ISR and their corresponding handlers
$
$INTNO_ATTISR_VALID = INTNO_VALID$
$INHNO_ATTISR_VALID = INTNO_VALID$

$
$  Number of Classes
$
$NUM_CLASS = 5$

$
$ CLASS 1 definitions
$

$  Initial processor ID
$CLASS_AFFINITY_INI[1] = 1$

$  Mask of processors to which migration is possible
$CLASS_AFFINITY_MASK[1] = 0x00000003$

$  Object lock
$ If P_LOCK definition of the processor that has the object lock
$CLASS_OBJ_LOCK[1] = 1$

$
$ CLASS 2 definitions
$
$CLASS_AFFINITY_INI[2] = 2$
$CLASS_AFFINITY_MASK[2] = 0x00000003$
$CLASS_OBJ_LOCK[2] = 2$

$
$ CLASS TCL_1_ONLY (CLASS ID 3) definitions
$
$CLASS_AFFINITY_INI[3] = 1$
$CLASS_AFFINITY_MASK[3] = 0x00000001$
$CLASS_OBJ_LOCK[3] = 1$

$
$ CLASS TCL_2_ONLY (CLASS ID 4) definitions
$
$CLASS_AFFINITY_INI[4] = 2$
$CLASS_AFFINITY_MASK[4] = 0x00000002$
$CLASS_OBJ_LOCK[4] = 2$

$
$ CLASS TCL_SYSTIM_PRC (CLASS ID 5) definitions
$
$CLASS_AFFINITY_INI[5] = TOPPERS_SYSTIM_PRCID$
$CLASS_AFFINITY_MASK[5] = 0x1 << (TOPPERS_SYSTIM_PRCID - 1) $
$CLASS_OBJ_LOCK[5] = TOPPERS_SYSTIM_PRCID$

$
$  Include chip template
$
$INCLUDE"../../arch/arm_gcc/mpcore/ca9.tf"$
