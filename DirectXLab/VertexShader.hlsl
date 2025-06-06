#include "Common.hlsli"

cbuffer ObjectData : register(b0)
{
    float4x4 world;
}

cbuffer PassData : register(b1)
{
    float4x4 viewProj;
}

VertexOut main( VertexIn vIn )
{
    float4x4 worldViewProj = mul(world, viewProj);

    VertexOut vOut;
    vOut.posH = mul(float4(vIn.posL, 1.0f), worldViewProj);
    vOut.color = vIn.color;
    vOut.uv = vIn.uv;

    return vOut;
}