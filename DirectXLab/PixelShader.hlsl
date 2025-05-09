#include "Common.hlsli"

float4 main(VertexOut vIn) : SV_TARGET
{

    return float4(vIn.uv, vIn.instanceID, 1.0f);
}