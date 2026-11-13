#include <Common.S>

// 805a34b0
PATCH_B_START(Camera_calcCamera, 0x22C)
    stwu r1, -0x40(r1)

    // Save registers that the patch needs to preserve
    stw r3, 0x08(r1)
    stfd f1, 0x10(r1)

    // arg 1: &orbitDir
    addi r3, r1, 0xEC

    // arg 2: &vStack_178 (yaw axis)
    addi r4, r1, 0xF8

    // arg 3: &vStack_16c (pitch axis)
    addi r5, r1, 0x104

    // arg 4: KartObjectProxy*
    mr r6, r26

    bl KartCamera_ApplyStickMovement

    // Restore saved registers
    lwz r3, 0x08(r1)
    lfd f1, 0x10(r1)

    // Restore stack
    addi r1, r1, 0x40

    // Replay original instruction, if this is the instruction
    // overwritten by the patch
    addi r3, r1, 0xAC

    b Camera_calcCamera + 0x22C + 4
PATCH_B_END(Camera_calcCamera, 0x22C)