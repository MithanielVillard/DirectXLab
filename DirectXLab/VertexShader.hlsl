#include "Common.hlsli"

VertexOut main( VertexIn vIn )
{
    VertexOut vOut;
    vOut.posH = float4(vIn.posL, 1.0f);
    vOut.color = vIn.color;
    vOut.uv = vIn.uv;

    return vOut;
}