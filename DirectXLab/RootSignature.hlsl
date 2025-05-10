// 0 - ObjectData - b0
// 1 - PassData - b1
// 2 - Simulation Time - b2
// 3 - Descriptor Table ParticleSystem


#define ROOTSIG \
"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), " \
"CBV(b0), " \
"CBV(b1), " \
"RootConstants(num32BitConstants=1, b2), " \
"DescriptorTable(SRV(t0), " \
                "UAV(u0), " \
                "CBV(b3))" 