#include "Common.hlsli"

float4 main(VertexOut vIn) : SV_TARGET
{
    return vIn.color;
}