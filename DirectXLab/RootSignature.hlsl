// 0 - ObjectData - b0
// 1 - PassData - b1
// 2 - Compute src - t0
// 3 - Compute out - u0


#define ROOTSIG \
"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), " \
"CBV(b0), " \
"CBV(b1), " \
"SRV(t0, visibility = SHADER_VISIBILITY_ALL), " \
"UAV(u0, visibility = SHADER_VISIBILITY_ALL)"