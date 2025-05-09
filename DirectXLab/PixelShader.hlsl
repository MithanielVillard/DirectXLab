#include "Common.hlsli"

RWStructuredBuffer<int> srcBuffer : register(u0);

float4 main(VertexOut vIn) : SV_TARGET
{

    return float4(vIn.uv, vIn.instanceID, 1.0f);
}