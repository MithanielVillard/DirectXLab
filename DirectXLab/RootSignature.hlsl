// 0 - ObjectData - b0
// 1 - PassData - b1
// 2 - compute read - t0
// 3 - Compute out - u0
// 4 - Simulation time - b2


#define ROOTSIG \
"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), " \
"CBV(b0), " \
"CBV(b1), " \
"SRV(t0), " \
"UAV(u0), " \
"RootConstants(num32BitConstants=1, b2)"